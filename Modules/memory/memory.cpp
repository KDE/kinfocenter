
/*
 *  memory.cpp
 *
 *  Copyright (C) 2008 Ivo Anjo <knuckles@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "memory.h"


#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <kaboutdata.h>

#include <sys/param.h>		/* for BSD */

#include <KLocalizedString>

#include <KPluginFactory>
#include <KPluginLoader>

#include "chartWidget.h"

#include "physicalMemoryChart.h"
#include "totalMemoryChart.h"
#include "swapMemoryChart.h"

/*
 all fetchValues()-functions should put either
 their results _OR_ the value NO_MEMORY_INFO into memoryInfos[]
 */
static t_memsize memoryInfos[MEM_LAST_ENTRY];

/******************/
/* Implementation */
/******************/

static QLabel *memorySizeLabels[MEM_LAST_ENTRY][2];


K_PLUGIN_FACTORY(KCMMemoryFactory,
		registerPlugin<KCMMemory>();
)

KCMMemory::KCMMemory(QWidget *parent, const QVariantList &) :
	KCModule(parent) {

	KAboutData *about = new KAboutData(i18n("kcm_memory"),
			i18n("KDE Panel Memory Information Control Module"),
			QString(), QString(), KAboutLicense::GPL,
			i18n("(c) 1998 - 2002 Helge Deller"));

	about->addAuthor(i18n("Helge Deller"), QString(), QStringLiteral("deller@gmx.de"));
	setAboutData(about);

	QString title, initial_str;

	setButtons(Help);

	QVBoxLayout *top = new QVBoxLayout(this);
	top->setContentsMargins(0, 0, 0, 0);
	top->setSpacing(1);

	QGroupBox* informationGroup = initializeText();
	top->addWidget(informationGroup, 1);

	// Now the Graphics
	QGroupBox* graphicsGroup = initializeCharts();
	top->addWidget(graphicsGroup, 2);

	timer = new QTimer(this);
	timer->start(100);

	connect(timer, &QTimer::timeout, this, &KCMMemory::updateDatas);
	
	updateDatas();
}

KCMMemory::~KCMMemory() {
	/* stop the timer */
	timer->stop();
}

QString KCMMemory::quickHelp() const {
	return i18n("This display shows you the current memory usage of your system."
		" The values are updated on a regular basis and give you an"
		" overview of the physical and virtual memory being used.");
}

QGroupBox* KCMMemory::initializeText() {
	QGroupBox* informationGroup = new QGroupBox(i18n("Memory"));

	QHBoxLayout *hbox = new QHBoxLayout(informationGroup);

	/* stretch the left side */
	hbox->addStretch();

	QString title;

	//TODO Use the more smart QGridLayout !!!

	/* first create the Informationtext-Widget */
	QVBoxLayout *vbox = new QVBoxLayout();
	hbox->addLayout(vbox);
	vbox->setSpacing(0);
	for (int i = TOTAL_MEM; i < MEM_LAST_ENTRY; ++i) {
		switch (i) {
		case TOTAL_MEM:
			title = i18n("Total physical memory:");
			break;
		case FREE_MEM:
			title = i18n("Free physical memory:");
			break;
#if !defined(__NetBSD__) && !defined(__OpenBSD__)
		case SHARED_MEM:
			title = i18n("Shared memory:");
			break;
		case BUFFER_MEM:
			title = i18n("Disk buffers:");
			break;
#else
			case ACTIVE_MEM:
			title = i18n("Active memory:");
			break;
			case INACTIVE_MEM:
			title = i18n("Inactive memory:");
			break;
#endif
		case CACHED_MEM:
			title = i18n("Disk cache:");
			break;
		case SWAP_MEM:
			vbox->addSpacing(SPACING);
			title = i18n("Total swap memory:");
			break;
		case FREESWAP_MEM:
			title = i18n("Free swap memory:");
			break;
		default:
			title = QLatin1String("");
			break;
		};
		QLabel* labelWidget = new QLabel(title, this);
		labelWidget->setTextInteractionFlags(Qt::TextSelectableByMouse);
		labelWidget->setAlignment(Qt::AlignLeft);
		vbox->addWidget(labelWidget);
	}

	vbox->addStretch();

	/* then the memory-content-widgets */
	for (int j = 0; j < 2; j++) {
		vbox = new QVBoxLayout();
		hbox->addLayout(vbox);
		vbox->setSpacing(0);
		for (int i = TOTAL_MEM; i < MEM_LAST_ENTRY; ++i) {
			if (i == SWAP_MEM)
				vbox->addSpacing(SPACING);
			QLabel* labelWidget = new QLabel(this);
			labelWidget->setTextInteractionFlags(Qt::TextSelectableByMouse);
			labelWidget->setAlignment(Qt::AlignRight);
			memorySizeLabels[i][j] = labelWidget;
			vbox->addWidget(labelWidget);
		}

		vbox->addStretch();

	}

	/* stretch the right side */
	hbox->addStretch();

	return informationGroup;

}

QGroupBox* KCMMemory::initializeCharts() {
	QGroupBox* chartsGroup = new QGroupBox(i18n("Charts"));

	QHBoxLayout* chartsLayout = new QHBoxLayout(chartsGroup);
	chartsLayout->setSpacing(1);
	chartsLayout->setContentsMargins(1, 1, 1, 1);

	//chartsLayout->addStretch(1);


	
	totalMemory = new ChartWidget(i18n("Total Memory"), 
			i18n("This graph gives you an overview of the "
			"<b>total sum of physical and virtual memory</b> "
			"in your system."), 
			new TotalMemoryChart(this), this);
	
	chartsLayout->addWidget(totalMemory);
	chartsLayout->addSpacing(SPACING);


	physicalMemory = new ChartWidget(i18n("Physical Memory"), 
			i18n("This graph gives you an overview of "
					"the <b>usage of physical memory</b> in your system."
					"<p>Most operating systems (including Linux) "
					"will use as much of the available physical "
					"memory as possible as disk cache, "
					"to speed up the system performance.</p>"
					"<p>This means that if you have a small amount "
					"of <b>Free Physical Memory</b> and a large amount of "
					"<b>Disk Cache Memory</b>, your system is well "
					"configured.</p>"), 
			new PhysicalMemoryChart(this), this);
	
	chartsLayout->addWidget(physicalMemory);
	chartsLayout->addSpacing(SPACING);

	swapMemory = new ChartWidget(i18n("Swap Space"), 
			i18n("<p>The swap space is the <b>virtual memory</b> "
				"available to the system.</p> "
				"<p>It will be used on demand and is provided "
				"through one or more swap partitions and/or swap files.</p>"), 
			new SwapMemoryChart(this), this);

	
	chartsLayout->addWidget(swapMemory);

	//chartsLayout->addStretch(1);

	return chartsGroup;
}

void KCMMemory::updateDatas() {

	/* get the Information from memory_linux, memory_fbsd */
	fetchValues(); 
	
	updateMemoryText();
	updateMemoryGraphics();
}


void KCMMemory::updateMemoryText() {
	/* update the byte-strings */
	for (int i = TOTAL_MEM; i < MEM_LAST_ENTRY; i++) {
		QLabel* label = memorySizeLabels[i][0];
		if (memoryInfos[i] == NO_MEMORY_INFO)
			label->clear();
		else
			label->setText(i18np("1 byte =", "%1 bytes =", memoryInfos[i]));
	}

	/* update the MB-strings */
	for (int i = TOTAL_MEM; i < MEM_LAST_ENTRY; i++) {
		QLabel* label = memorySizeLabels[i][1];
		label->setText((memoryInfos[i] != NO_MEMORY_INFO) ? Chart::formattedUnit(memoryInfos[i]) : i18n("Not available."));
	}

}

void KCMMemory::updateMemoryGraphics() {
	totalMemory->setMemoryInfos(memoryInfos);
	totalMemory->refresh();

	physicalMemory->setMemoryInfos(memoryInfos);
	physicalMemory->refresh();

	swapMemory->setMemoryInfos(memoryInfos);
	swapMemory->refresh();

}

/* Include system-specific code */

#ifdef __linux__
#include "memory_linux.cpp"
#elif defined(__FreeBSD__) || defined(__DragonFly__)
#include "memory_fbsd.cpp"
#elif defined(__NetBSD__) || defined(__OpenBSD__)
#include "memory_netbsd.cpp"
#else

/* Default for unsupported systems */
void KCMMemory::fetchValues() {
	int i;
	for (i = TOTAL_MEM; i < MEM_LAST_ENTRY; ++i) {
		memoryInfos[i] = NO_MEMORY_INFO;
	}
}

#endif

#include "memory.moc"
