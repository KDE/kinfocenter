/*
 *  info_solaris.cpp
 *
 *  Torsten Kasch <tk@Genetik.Uni-Bielefeld.DE>
 */

#include "config-infocenter.h"

#include <QTreeWidgetItemIterator>

#include <stdio.h>
#include <stdlib.h>
#include <sys/mnttab.h>
#include <kstat.h>
#include <sys/types.h>
#include <sys/statvfs.h>
#include <time.h>

#ifdef HAVE_LIBDEVINFO_H
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <sys/mkdev.h>
#include <sys/stat.h>
#include <devid.h>
#include <libdevinfo.h>
#endif /* HAVE_LIBDEVINFO_H */

bool GetInfo_IRQ(QTreeWidget*) {
	return false;
}

bool GetInfo_DMA(QTreeWidget*) {
	return false;
}

bool GetInfo_PCI(QTreeWidget*) {
	return false;
}

bool GetInfo_IO_Ports(QTreeWidget*) {
	return false;
}

bool GetInfo_Sound(QTreeWidget*) {
	return false;
}

bool GetInfo_SCSI(QTreeWidget*) {
	return false;
}

bool GetInfo_Partitions(QTreeWidget* tree) {

	FILE *mnttab;
	struct mnttab mnt;
	struct statvfs statbuf;
	fsblkcnt_t tmp;
	QString total;
	QString avail;
	time_t mnttime;
	char *timetxt;
	char *ptr;

	if ( (mnttab = fopen(MNTTAB, "r")) == NULL) {
		return false;
	}

	/*
	 *  set up column headers
	 */
	QStringList headers;
	headers << i18n("Device") << i18n("Mount Point") << i18n("FS Type") << i18n("Total Size") << i18n("Free Size") << i18n("Mount Time") << i18n("Mount Options");
	tree->setHeaderLabels(headers);

	// XXX: FIXME: how do I set column alignment correctly?
	//tree->setColumnAlignment( 3, 2 );
	// XXX: FIXME: how do I set column alignment correctly?
	//tree->setColumnAlignment( 4, 2 );

	/*
	 *  get info about mounted file systems
	 */
	rewind(mnttab);
	while (getmntent(mnttab, &mnt) == 0) {
		/*
		 *  skip fstype "nfs" and "autofs" for two reasons:
		 *	o if the mountpoint is visible, the fs is not
		 *	  necessarily available (autofs option "-nobrowse")
		 *	  and we don't want to mount every remote fs just
		 *	  to get its size, do we?
		 *	o the name "Partitions" for this statistics implies
		 *	  "local file systems only"
		 */
		if ( (strcmp(mnt.mnt_fstype, "nfs") == 0) || (strcmp(mnt.mnt_fstype, "autofs") == 0))
			continue;
		if (statvfs(mnt.mnt_mountp, &statbuf) == 0) {
			if (statbuf.f_blocks > 0) {
				/*
				 *  produce output in KB, MB, or GB for
				 *  readability -- unfortunately, this
				 *  breaks sorting for these columns...
				 */
				tmp = statbuf.f_blocks * (statbuf.f_frsize / 1024);
				if (tmp > 9999) {
					tmp /= 1024;
					if (tmp > 9999) {
						tmp /= 1024;
						total.setNum(tmp);
						total += " G";
					} else {
						total.setNum(tmp);
						total += " M";
					}
				} else {
					total.setNum(tmp);
					total += " K";
				}
				//				avail.setNum( statbuf.f_bavail );
				//				avail += " K";
				tmp = statbuf.f_bavail * (statbuf.f_frsize / 1024);
				if (tmp > 9999) {
					tmp /= 1024;
					if (tmp > 9999) {
						tmp /= 1024;
						avail.setNum(tmp);
						avail += " G";
					} else {
						avail.setNum(tmp);
						avail += " M";
					}
				} else {
					avail.setNum(tmp);
					avail += " K";
				}
			} else {
				total = "-";
				avail = "-";
			}
		} else {
			total = "???";
			avail = "???";
		}
		/*
		 *  ctime() adds a '\n' which we have to remove
		 *  so that we get a one-line output for the QTreeWidgetItem
		 */
		mnttime = (time_t) atol(mnt.mnt_time);
		if ( (timetxt = ctime( &mnttime)) != NULL) {
			ptr = strrchr(timetxt, '\n');
			*ptr = '\0';
		}

		QStringList list;
		list << mnt.mnt_special << mnt.mnt_mountp << mnt.mnt_fstype << total << avail << QString(timetxt) << mnt.mnt_mntopts;
		new QTreeWidgetItem(tree, list);
	}
	fclose(mnttab);

	tree->sortItems(0, Qt::AscendingOrder);

	return true;
}

bool GetInfo_XServer_and_Video(QTreeWidget* tree) {
	return GetInfo_XServer_Generic(tree);
}

#ifdef HAVE_LIBDEVINFO_H
/*
 *  get Solaris' device configuration data through libdevinfo(3)
 *  and display it in a prtconf(1M) style tree
 *
 *  NOTE: though the devinfo library seems to be present on earlier
 *        Solaris releases, this interface is documented to be available
 *        since Solaris 7 (libdevinfo.h is missing on pre-Solaris 7 systems)
 *
 *  documentation for libdevinfo(3) including code samples on which
 *  this implementation is based on is available at
 *	http://soldc.sun.com/developer/support/driver/wps/libdevinfo/
 */

/*
 *  we start with various helper routines for GetInfo_Devices()
 */

/*
 *  mktree() -- break up the device path and place its components
 *		into the tree widget
 */
QTreeWidgetItem* mktree( QTreeWidgetItem* top, const char* path ) {

	QTreeWidgetItem* parent;
	QTreeWidgetItem* previous;
	QTreeWidgetItem* result;
	char* str = strdup( path );
	char* token;

	/*
	 *  start at "/"
	 */
	parent = top;
	result = top->child(0);
	previous = top->child(0);

	token = strtok( str, "/" );
	while( token != NULL ) {
		/*
		 *  find insert pos:
		 *  try to match the node at the current level
		 *
		 *  NOTE: this implementation assumes that there are
		 *        no two nodes with identical names at the
		 *        same level of the device tree
		 */
		QTreeWidgetItemIterator it(top, QTreeWidgetItemIterator::All);
		while ( *it != NULL ) {
			if( strcmp( token, (*it)->text( 0 ).toLatin1()) == 0 )
			break;

			previous = *it;

			++it;
			result = *it;
		}
		if( result == NULL ) {
			QStringList list;
			list << token;

			/*
			 *  we haven't found the node, create a new one
			 */
			result = new QTreeWidgetItem(parent, previous, list);
		}
		else {
			/*
			 *  we've found the node
			 */
			parent = result;
			previous = NULL;
			if( result->child(0) == NULL ) {
				/*
				 *  create new node during next iteration
				 */
				result->setExpanded(false);
			} else {
				/*
				 *  follow the child path
				 */
				result = result->child(0);
			}
		}
		token = strtok( NULL, "/" );
	}

	free( str );

	return result;
}

/*
 *  prop_type_str()  -- return the property type as a string
 */
char *prop_type_str( di_prop_t prop ) {

	switch( di_prop_type( prop )) {
		case DI_PROP_TYPE_UNDEF_IT:
		return( "undefined" );
		case DI_PROP_TYPE_BOOLEAN:
		return( "BOOL" );
		case DI_PROP_TYPE_INT:
		return( "INT" );
		case DI_PROP_TYPE_STRING:
		return( "STRING" );
		case DI_PROP_TYPE_BYTE:
		return( "BYTE" );
		default:
		return( "unknown" );
	}
}

/*
 *  prop_type_guess() -- guess the property type
 */
int prop_type_guess( uchar_t *data, int len ) {

	int slen;
	int guess;
	int i, c;

	if( len < 0 )
	return( -1 );
	else if( len == 0 )
	return( DI_PROP_TYPE_BOOLEAN );

	slen = 0;
	guess = DI_PROP_TYPE_STRING;

	for( i = 0; i < len; i++ ) {
		c = (int) data[i];
		switch( c ) {
			case 0:
			if( i == (len - 1 ))
			break;
			if( slen == 0 )
			guess = DI_PROP_TYPE_BYTE;
			else
			guess = slen = 0;
			break;
			default:
			if( ! isprint( c ))
			guess = DI_PROP_TYPE_BYTE;
			else
			slen++;
		}
		if( guess != DI_PROP_TYPE_STRING )
		break;
	}

	//	if( (guess == DI_PROP_TYPE_BYTE) && (len % sizeof( int ) == 0 ))
	//		guess = DI_PROP_TYPE_INT;

	return( guess );
}

/*
 *  dump_minor_node()  --  examine a device minor node
 *			   this routine gets passed to di_walk_node()
 */
int dump_minor_node( di_node_t node, di_minor_t minor, void *arg ) {

	QString majmin;
	char *type;
	dev_t dev;

	QStringList list;
	list << di_minor_name( minor );
	QTreeWidgetItem* item = new QTreeWidgetItem( (QTreeWidgetItem *) arg, list);
	item->setExpanded(false);

	QString specResult;
	if (di_minor_spectype( minor ) == S_IFCHR)
	specResult = i18n( "character special" );
	else
	specResult = i18n( "block special" );

	QStringList spec;
	spec << i18nc( "Can be either character special or block special", "Special type:" ) << specResult;

	new QTreeWidgetItem(item, spec);

	type = di_minor_nodetype( minor );

	QString nodeResult;
	if (type == NULL)
	nodeResult = "NULL";
	else
	nodeResult = type;

	QStringList node;
	node << i18n( "Nodetype:" ) << nodeResult;

	new QTreeWidgetItem(item, node);

	if( (dev = di_minor_devt( minor )) != DDI_DEV_T_NONE ) {
		majmin.sprintf( "%ld/%ld", major( dev ), minor( dev ));
		QStringList mn;
		mn << i18n( "Major/Minor:" ) << majmin;
		new QTreeWidgetItem(item, mn);
	}

	if( di_minor_next( node, minor ) == DI_MINOR_NIL )
	return DI_WALK_TERMINATE;
	else
	return DI_WALK_CONTINUE;
}

/*
 *  propvalue() -- return the property value
 */
QString propvalue( di_prop_t prop ) {

	int type;
	int i, n;
	char *strp;
	int *intp;
	uchar_t *bytep;
	QString result;

	/*
	 *  Since a lot of printable strings seem to be tagged as 'byte',
	 *  we're going to guess, if the property is not STRING or INT
	 *  The actual type is shown in the info tree, though.
	 */
	type = di_prop_type( prop );
	if( (type != DI_PROP_TYPE_STRING) && (type != DI_PROP_TYPE_INT) ) {
		n = di_prop_bytes( prop, &bytep );
		type = prop_type_guess( bytep, n );
	}

	result = "";
	switch( type ) {
		case DI_PROP_TYPE_STRING:
		if( (n = di_prop_strings( prop, &strp )) < 0 ) {
			result = "(error)";
		} else {
			for( i = 0; i < n; i++ ) {
				result += "\"";
				result += strp;
				result += "\" ";
				strp += strlen( strp ) + 1;
			}
		}
		break;
		case DI_PROP_TYPE_INT:
		if( (n = di_prop_ints( prop, &intp )) < 0 ) {
			result = "(error)";
		} else {
			for( i = 0; i < n; i++ ) {
				QString tmp;
				tmp.setNum( intp[i] );
				result += tmp;
				result += ' ';
			}
		}
		break;
		case DI_PROP_TYPE_BOOLEAN:
		/*
		 *  hmm, Sun's sample code handles the existence
		 *  of a boolean property as "true", whereas
		 *  prtconf(1M) obviously does not (Sol8, at least)
		 *  -- we're doing the same and handle "bool" as "byte"
		 */
		case DI_PROP_TYPE_BYTE:
		if( (n = di_prop_bytes( prop, &bytep )) < 0 ) {
			result = "(error)";
		} else {
			if( n == 0 ) {
				result = i18n( "(no value)" );
				break;
			}
			result = "0x";
			for( i = 0; i < n; i++ ) {
				QString tmp;
				unsigned byte = (unsigned) bytep[i];
				tmp.sprintf( "%2.2x", byte );
				result += tmp;
			}
		}
		break;
		default:
		result = "???";
	}

	return( result );
}

/*
 *  dump_node() -- examine a device node and its children
 *		   this routine gets passed to di_walk_node()
 */
int dump_node( di_node_t node, void *arg ) {

	QTreeWidgetItem *top = (QTreeWidgetItem *) arg;
	QTreeWidgetItem *parent;
	char *path;
	char *drivername;
	char *names;
	QString compatnames;
	int i, n;
	di_prop_t prop;

	path = di_devfs_path( node );

	/*
	 *  if this is the root node ("/"), initialize the tree
	 */
	if( strlen( path ) == 1 ) {
		top->setText( 0, QString( di_binding_name( node )));
		top->setIcon( 0, SmallIcon( "kcmdevices" ));
		top->setExpanded( true );
	}

	/*
	 *  place the node in the tree
	 */
	parent = mktree( top, path );

	/*
	 *  we have to handle the root node differently...
	 */
	if( strlen( path ) > 1 ) {
		parent->setExpanded(false);
	} else {
		parent = top;
	}

	/*
	 *  node name and physical device path
	 */
	drivername = di_driver_name( node );

	QStringList driverList;
	driverList << i18n( "Driver Name:" );
	if (drivername==NULL)
	driverList << i18n( "(driver not attached)" );
	else
	driverList << drivername;

	new QTreeWidgetItem(parent, driverList);

	QStringList bindingList;
	bindingList << i18n( "Binding Name:" ) << di_binding_name( node );
	new QTreeWidgetItem(parent, bindingList);

	n = di_compatible_names( node, &names );
	if( n < 1 ) {
		compatnames = i18n( "(none)" );
	} else {
		for( i = 0; i < n; i++ ) {
			compatnames += names;
			compatnames += ' ';
			names += strlen( names ) + 1;
		}
	}

	QStringList compatibleList;
	compatibleList << i18n( "Compatible Names:" ) << compatnames;
	new QTreeWidgetItem(parent, compatibleList);

	QStringList physicalList;
	physicalList << i18n( "Physical Path:" ) << QString( path );
	new QTreeWidgetItem(parent, physicalList);

	/*
	 *  dump the node's property list (if any)
	 */
	if( (prop = di_prop_next( node, DI_PROP_NIL )) != DI_PROP_NIL ) {

		QTreeWidgetItem *previous;
		QStringList propertiesList;
		propertiesList << i18n( "Properties" );
		previous = new QTreeWidgetItem(parent, propertiesList);
		previous->setExpanded( false );

		do {
			/*
			 *  property type & value
			 */
			QStringList propList;
			propList << di_prop_name( prop );
			QTreeWidgetItem* tmp = new QTreeWidgetItem(previous, propList);
			tmp->setExpanded( false );

			QStringList typeList;
			typeList << i18n( "Type:" ) << prop_type_str( prop );
			new QTreeWidgetItem( tmp, typeList);

			QStringList valueList;
			valueList << i18n( "Value:" ) << propvalue( prop );
			new QTreeWidgetItem( tmp, valueList);

		}while( (prop = di_prop_next( node, prop )) != DI_PROP_NIL );
	}

	/*
	 *  if there are minor nodes, expand the tree appropriately
	 */
	if( di_minor_next( node, DI_MINOR_NIL ) != DI_MINOR_NIL ) {
		QStringList minorList;
		minorList << i18n( "Minor Nodes" );
		QTreeWidgetItem* previous = new QTreeWidgetItem(parent, minorList);
		previous->setExpanded(false);
		di_walk_minor( node, NULL, 0, previous, dump_minor_node );
	}

	return DI_WALK_CONTINUE;
}

bool GetInfo_Devices( QTreeWidget* tree ) {

	di_node_t root_node;

	/*
	 *  create a snapshot of the device tree
	 */
	if( (root_node = di_init( "/", DINFOCPYALL )) == DI_NODE_NIL ) {
		return( false );
	}
	// XXX: might try to di_prom_init() here as well (if we're setgid sys)

	/*
	 *  prepare the tree widget
	 */
	QStringList headers;
	headers << i18n( "Device Information" ) << i18n( "Value" );
	tree->setHeaderLabels(headers);

	QTreeWidgetItem* top = new QTreeWidgetItem( tree );

	/*
	 *  traverse the device tree
	 */
	di_walk_node( root_node, DI_WALK_CLDFIRST, top, dump_node );

	di_fini( root_node );

	tree->setSortingEnabled(false);

	return true;
}

#else /* ! HAVE_LIBDEVINFO_H */
bool GetInfo_Devices(QTreeWidget*) {
	return false;
}
#endif /* ! HAVE_LIBDEVINFO_H */
