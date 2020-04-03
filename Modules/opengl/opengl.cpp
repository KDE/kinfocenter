/*
 *  opengl.cpp
 *
 *  Copyright (C) 2008 Ivo Anjo <knuckles@gmail.com>
 *  Copyright (C) 2004 Ilya Korniyko <k_ilya@ukr.net>
 *  Adapted from Brian Paul's glxinfo from Mesa demos (https://www.mesa3d.org/)
 *  Copyright (C) 1999-2002 Brian Paul
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

#include "opengl.h"

#include <QRegExp>
#include <QFile>
#include <QTextStream>
#include <QProcess>
#include <QDebug>
#include <QOpenGLContext>
#include <QOffscreenSurface>
#include <QSurfaceFormat>
#include <QDirIterator>

#include <KPluginFactory>
#include <KPluginLoader>

#include <kaboutdata.h>
#include <KLocalizedString>
#include <kmessagebox.h>

// X11 includes
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <openglconfig.h>

#if KCM_HAVE_EGL
#include <EGL/egl.h>
#endif

#ifdef KCM_ENABLE_OPENGLES
#include <GLES2/gl2.h>
#endif

#if KCM_HAVE_GLX
// GLU includes
#include <GL/glu.h>

// OpenGL includes
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glx.h>
#endif

#if defined(Q_OS_LINUX)
#include <QFileInfo>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#endif

K_PLUGIN_FACTORY(KCMOpenGLFactory,
    registerPlugin<KCMOpenGL>();
)

// FIXME: Temporary!
bool GetInfo_OpenGL(QTreeWidget *treeWidget);

KCMOpenGL::KCMOpenGL(QWidget *parent, const QVariantList &)
    : KCModule(parent)
{
    setupUi(this);
    layout()->setContentsMargins(0, 0, 0, 0);
    
    GetInfo_OpenGL(glinfoTreeWidget);

    // Watch for expanded and collapsed events, to resize columns
    connect(glinfoTreeWidget, &QTreeWidget::expanded, this, &KCMOpenGL::treeWidgetChanged);
    connect(glinfoTreeWidget, &QTreeWidget::collapsed, this, &KCMOpenGL::treeWidgetChanged);
    
    KAboutData *about =
    new KAboutData(i18n("kcmopengl"),
        i18n("KCM OpenGL Information"),
        QString(), QString(), KAboutLicense::GPL,
        i18n("(c) 2008 Ivo Anjo\n(c) 2004 Ilya Korniyko\n(c) 1999-2002 Brian Paul"));

    about->addAuthor(i18n("Ivo Anjo"), QString(), QStringLiteral("knuckles@gmail.com"));
    about->addAuthor(i18n("Ilya Korniyko"), QString(), QStringLiteral("k_ilya@ukr.net"));
    about->addCredit(i18n("Helge Deller"), i18n("Original Maintainer"), QStringLiteral("deller@gmx.de"));
    about->addCredit(i18n("Brian Paul"), i18n("Author of glxinfo Mesa demos (https://www.mesa3d.org)"));
    setAboutData(about);
}

void KCMOpenGL::treeWidgetChanged() {
    glinfoTreeWidget->resizeColumnToContents(0);
    glinfoTreeWidget->resizeColumnToContents(1);
}

QTreeWidgetItem *newItem(QTreeWidgetItem *parent, QTreeWidgetItem *preceding, const QString &textCol1, const QString &textCol2 = QString()) {
    QTreeWidgetItem *newItem;
    if ((parent == nullptr) && (preceding == nullptr)) {
	newItem = new QTreeWidgetItem();
    } else if (preceding == nullptr) {
	newItem = new QTreeWidgetItem(parent);
    } else {
	newItem = new QTreeWidgetItem(parent, preceding);
    }
    newItem->setText(0, textCol1);
    if (!textCol2.isNull()) {
	newItem->setText(1, textCol2);
    }
    
    newItem->setFlags(Qt::ItemIsEnabled);
    return newItem;
}

QTreeWidgetItem *newItem(QTreeWidgetItem *parent, const QString &textCol1, const QString &textCol2 = QString()) {
    return newItem(parent, NULL, textCol1, textCol2);
}

static bool IsDirect;

static struct glinfo {
#if KCM_HAVE_GLX
      const char *serverVendor;
      const char *serverVersion;
      const char *serverExtensions;
      const char *clientVendor;
      const char *clientVersion;
      const char *clientExtensions;
      const char *glxExtensions;
#endif
#if KCM_HAVE_EGL
      const char *eglVendor;
      const char *eglVersion;
      const char *eglExtensions;
#endif
      const char *glVendor;
      const char *glRenderer;
      const char *glVersion;
      const char *glExtensions;
#if KCM_HAVE_GLX
      const char *gluVersion;
      const char *gluExtensions;
#endif
      char *displayName;
} gli;

struct DriInfo {
    QString deviceId; // card0 etc.
    QString module;
    QString pci;
    QString vendor;
    QString device;
    QString subvendor;
    QString rev;

    bool isValid() const
    {
        return (!module.isEmpty() && !pci.isEmpty() && !deviceId.isEmpty());
    }
};

static int ReadPipe(const QString &FileName, QStringList &list)
{
    QProcess pipe;
    pipe.start(FileName, QIODevice::ReadOnly);

    if (!pipe.waitForFinished()) {
	// something went wrong, f.e. command not found
	return 0;
    }

    QTextStream t(&pipe);

    while (!t.atEnd()) list.append(t.readLine());

    return list.count();
}

#if defined(Q_OS_LINUX)

static QString get_sysfs_link_name(const QString &path)
{
    const QString target = QFileInfo(path).symLinkTarget();

    const int index = target.lastIndexOf(QChar('/'));
    if (index == -1)
        return QString();

    return target.mid(index + 1);
}

static DriInfo get_drm_device_sysfs(const QString &path)
{
    DriInfo info;

    struct stat fileInfo{};
    if (::stat(QFile::encodeName(path), &fileInfo) != 0) {
        return info;
    }
    if ((fileInfo.st_mode & S_IFCHR) != S_IFCHR) {
        return info;
    }

    const uint16_t devMajor = major(fileInfo.st_rdev);
    const uint16_t devMinor = minor(fileInfo.st_rdev);
    const QString sysPath = QStringLiteral("/sys/dev/char/%1:%2").arg(devMajor).arg(devMinor);

    info.pci = get_sysfs_link_name(sysPath + QStringLiteral("/device"));
    info.module = get_sysfs_link_name(sysPath + QStringLiteral("/device/driver"));

    return info;
}

static QVector<DriInfo> get_drm_devices_sysfs()
{
    QVector<DriInfo> list;
    QDirIterator it("/dev/dri/", {"card*"}, QDir::System);
    while (it.hasNext()) {
        it.next();
        auto info = get_drm_device_sysfs(it.filePath());
        info.deviceId = it.fileName();
        if (info.isValid()) {
            list << info;
        }
    }
    std::sort(list.begin(), list.end(), [](const DriInfo &i1, const DriInfo &i2) -> bool {
        // reverse card0 < card1 < card2
        return QString::localeAwareCompare(i1.deviceId, i2.deviceId) < 0;
    });
    return list;
}

static QVector<DriInfo> get_dri_devices()
{
    auto infos = get_drm_devices_sysfs();
    for (auto it = infos.begin(); it != infos.end(); ++it) {
        const QString cmd = QStringLiteral("lspci -m -v -s ") + it->pci;
        QStringList pci_info;
        int num = 0;
        if (((num = ReadPipe(cmd, pci_info)) ||
             (num = ReadPipe("/sbin/"+cmd, pci_info)) ||
             (num = ReadPipe("/usr/sbin/"+cmd, pci_info)) ||
             (num = ReadPipe("/usr/local/sbin/"+cmd, pci_info))) && num>=7) {
            QString line;
            for (int i=2; i<=6; i++) {
                line = pci_info[i];
                line.remove(QRegExp("[^:]*:[ ]*"));
                switch (i) {
                    case 2: it->vendor = line.simplified();    break;
                    case 3: it->device = line.simplified();    break;
                    case 4: it->subvendor = line.simplified(); break;
                    case 6: it->rev = line.simplified();       break;
                }
            }
        } else {
            qDebug() << "failed to pcinfo" << cmd;;
            it = infos.erase(it);
            if (it == infos.end()) {
                break;
            }
        }
    }
    return infos;
}

#elif defined(Q_OS_FREEBSD)

static QVector<DriInfo> get_dri_devices() {
    QVector<DriInfo> list;
	QStringList pci_info;
    if (ReadPipe("sysctl -n hw.dri.0.name", pci_info)) {
        DriInfo info;
        info.module = pci_info[0].mid(0, pci_info[0].indexOf(0x20));
        list << info;
    }
    return list;
}

#else

static QVector<DriInfo> get_dri_devices() { return {}; }

#endif

#if KCM_HAVE_GLX
static void
mesa_hack(Display *dpy, int scrnum)
{
   static const int attribs[] = {
      GLX_RGBA,
      GLX_RED_SIZE, 1,
      GLX_GREEN_SIZE, 1,
      GLX_BLUE_SIZE, 1,
      GLX_DEPTH_SIZE, 1,
      GLX_STENCIL_SIZE, 1,
      GLX_ACCUM_RED_SIZE, 1,
      GLX_ACCUM_GREEN_SIZE, 1,
      GLX_ACCUM_BLUE_SIZE, 1,
      GLX_ACCUM_ALPHA_SIZE, 1,
      GLX_DOUBLEBUFFER,
      None
   };
   XVisualInfo *visinfo;

   visinfo = glXChooseVisual(dpy, scrnum, const_cast<int*>(attribs));
   if (visinfo)
      XFree(visinfo);
}

#endif

static void
print_extension_list(const char *ext, QTreeWidgetItem *l1)
{
   int i, j;

   if (!ext || !ext[0])
      return;
   QString qext = QString::fromLatin1(ext);
   QTreeWidgetItem *l2 = nullptr;

   i = j = 0;
   while (1) {
      if (ext[j] == ' ' || ext[j] == 0) {
         /* found end of an extension name */
         const int len = j - i;
         /* print the extension name between ext[i] and ext[j] */
	 if (!l2) l2 = newItem(l1, qext.mid(i, len));
	 else l2 = newItem(l1, l2, qext.mid(i, len));
	 i=j;
         if (ext[j] == 0) {
            break;
         }
         else {
            i++;
            j++;
            if (ext[j] == 0)
               break;
         }
      }
      j++;
   }
}

#if KCM_HAVE_GLX

#if defined(GLX_ARB_get_proc_address) && defined(__GLXextFuncPtr)
extern "C" {
	extern __GLXextFuncPtr glXGetProcAddressARB (const GLubyte *);
}
#endif

static void
print_limits(QTreeWidgetItem *l1, const char * glExtensions, bool getProcAddress)
{
 /*  TODO
      GL_SAMPLE_BUFFERS
      GL_SAMPLES
      GL_COMPRESSED_TEXTURE_FORMATS
*/

  if (!glExtensions)
	return;

  struct token_name {
      GLuint type;  // count and flags, !!! count must be <=2 for now
      GLenum token;
      const QString name;
   };

   struct token_group {
   	int count;
	int type;
	const token_name *group;
	const QString descr;
	const char *ext;
   };

   QTreeWidgetItem *l2 = nullptr, *l3 = nullptr;
#if defined(PFNGLGETPROGRAMIVARBPROC)
   PFNGLGETPROGRAMIVARBPROC kcm_glGetProgramivARB = nullptr;
#endif

   #define KCMGL_FLOAT 128
   #define KCMGL_PROG 256
   #define KCMGL_COUNT_MASK(x) (x & 127)
   #define KCMGL_SIZE(x) (sizeof(x)/sizeof(x[0]))

   const struct token_name various_limits[] = {
      { 1, GL_MAX_LIGHTS, 		i18n("Max. number of light sources") },
      { 1, GL_MAX_CLIP_PLANES,		i18n("Max. number of clipping planes") },
      { 1, GL_MAX_PIXEL_MAP_TABLE, 	i18n("Max. pixel map table size") },
      { 1, GL_MAX_LIST_NESTING, 	i18n("Max. display list nesting level") },
      { 1, GL_MAX_EVAL_ORDER, 		i18n("Max. evaluator order") },
      { 1, GL_MAX_ELEMENTS_VERTICES, 	i18n("Max. recommended vertex count") },
      { 1, GL_MAX_ELEMENTS_INDICES, 	i18n("Max. recommended index count") },
#ifdef GL_QUERY_COUNTER_BITS
      { 1, GL_QUERY_COUNTER_BITS, 	i18n("Occlusion query counter bits")},
#endif
#ifdef GL_MAX_VERTEX_UNITS_ARB
      { 1, GL_MAX_VERTEX_UNITS_ARB, 	i18n("Max. vertex blend matrices") },
#endif
#ifdef GL_MAX_PALETTE_MATRICES_ARB
      { 1, GL_MAX_PALETTE_MATRICES_ARB, i18n("Max. vertex blend matrix palette size") },
#endif
      {0,0,QString()}
     };

   const struct token_name texture_limits[] = {
      { 1, GL_MAX_TEXTURE_SIZE, 	i18n("Max. texture size") },
      { 1, GL_MAX_TEXTURE_UNITS_ARB, 	i18n("No. of texture units") },
      { 1, GL_MAX_3D_TEXTURE_SIZE, 		i18n("Max. 3D texture size") },
      { 1, GL_MAX_CUBE_MAP_TEXTURE_SIZE_ARB, 	i18n("Max. cube map texture size") },
#ifdef GL_MAX_RECTANGLE_TEXTURE_SIZE_NV
      { 1, GL_MAX_RECTANGLE_TEXTURE_SIZE_NV, 	i18n("Max. rectangular texture size") },
#endif
      { 1 | KCMGL_FLOAT, GL_MAX_TEXTURE_LOD_BIAS_EXT, i18n("Max. texture LOD bias") },
      { 1, GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, 	i18n("Max. anisotropy filtering level") },
      { 1, GL_NUM_COMPRESSED_TEXTURE_FORMATS_ARB, i18n("No. of compressed texture formats") },
      {0,0,QString()}
     };

   const struct token_name float_limits[] = {
      { 2 | KCMGL_FLOAT, GL_ALIASED_POINT_SIZE_RANGE, 	"ALIASED_POINT_SIZE_RANGE" },
      { 2 | KCMGL_FLOAT, GL_SMOOTH_POINT_SIZE_RANGE, 	"SMOOTH_POINT_SIZE_RANGE" },
      { 1 | KCMGL_FLOAT, GL_SMOOTH_POINT_SIZE_GRANULARITY,"SMOOTH_POINT_SIZE_GRANULARITY"},
      { 2 | KCMGL_FLOAT, GL_ALIASED_LINE_WIDTH_RANGE, 	"ALIASED_LINE_WIDTH_RANGE" },
      { 2 | KCMGL_FLOAT, GL_SMOOTH_LINE_WIDTH_RANGE, 	"SMOOTH_LINE_WIDTH_RANGE" },
      { 1 | KCMGL_FLOAT, GL_SMOOTH_LINE_WIDTH_GRANULARITY,"SMOOTH_LINE_WIDTH_GRANULARITY"},
      {0,0,QString()}
     };

   const struct token_name stack_depth[] = {
      { 1, GL_MAX_MODELVIEW_STACK_DEPTH, 	"MAX_MODELVIEW_STACK_DEPTH" },
      { 1, GL_MAX_PROJECTION_STACK_DEPTH, 	"MAX_PROJECTION_STACK_DEPTH" },
      { 1, GL_MAX_TEXTURE_STACK_DEPTH, 		"MAX_TEXTURE_STACK_DEPTH" },
      { 1, GL_MAX_NAME_STACK_DEPTH, 		"MAX_NAME_STACK_DEPTH" },
      { 1, GL_MAX_ATTRIB_STACK_DEPTH, 		"MAX_ATTRIB_STACK_DEPTH" },
      { 1, GL_MAX_CLIENT_ATTRIB_STACK_DEPTH, 	"MAX_CLIENT_ATTRIB_STACK_DEPTH" },
      { 1, GL_MAX_COLOR_MATRIX_STACK_DEPTH, 	"MAX_COLOR_MATRIX_STACK_DEPTH" },
#ifdef GL_MAX_MATRIX_PALETTE_STACK_DEPTH_ARB
      { 1, GL_MAX_MATRIX_PALETTE_STACK_DEPTH_ARB,"MAX_MATRIX_PALETTE_STACK_DEPTH"},
#endif
      {0,0,QString()}
   };

#ifdef GL_ARB_fragment_program
   const struct token_name arb_fp[] = {
    { 1, GL_MAX_TEXTURE_COORDS_ARB, "MAX_TEXTURE_COORDS" },
    { 1, GL_MAX_TEXTURE_IMAGE_UNITS_ARB, "MAX_TEXTURE_IMAGE_UNITS" },
    { 1 | KCMGL_PROG, GL_MAX_PROGRAM_ENV_PARAMETERS_ARB, "MAX_PROGRAM_ENV_PARAMETERS" },
    { 1 | KCMGL_PROG, GL_MAX_PROGRAM_LOCAL_PARAMETERS_ARB, "MAX_PROGRAM_LOCAL_PARAMETERS" },
    { 1, GL_MAX_PROGRAM_MATRICES_ARB, "MAX_PROGRAM_MATRICES" },
    { 1, GL_MAX_PROGRAM_MATRIX_STACK_DEPTH_ARB, "MAX_PROGRAM_MATRIX_STACK_DEPTH" },
    { 1 | KCMGL_PROG, GL_MAX_PROGRAM_INSTRUCTIONS_ARB, "MAX_PROGRAM_INSTRUCTIONS" },
    { 1 | KCMGL_PROG, GL_MAX_PROGRAM_ALU_INSTRUCTIONS_ARB, "MAX_PROGRAM_ALU_INSTRUCTIONS" },
    { 1 | KCMGL_PROG, GL_MAX_PROGRAM_TEX_INSTRUCTIONS_ARB, "MAX_PROGRAM_TEX_INSTRUCTIONS" },
    { 1 | KCMGL_PROG, GL_MAX_PROGRAM_TEX_INDIRECTIONS_ARB, "MAX_PROGRAM_TEX_INDIRECTIONS" },
    { 1 | KCMGL_PROG, GL_MAX_PROGRAM_TEMPORARIES_ARB, "MAX_PROGRAM_TEMPORARIES" },
    { 1 | KCMGL_PROG, GL_MAX_PROGRAM_PARAMETERS_ARB, "MAX_PROGRAM_PARAMETERS" },
    { 1 | KCMGL_PROG, GL_MAX_PROGRAM_ATTRIBS_ARB, "MAX_PROGRAM_ATTRIBS" },
    { 1 | KCMGL_PROG, GL_MAX_PROGRAM_NATIVE_INSTRUCTIONS_ARB, "MAX_PROGRAM_NATIVE_INSTRUCTIONS" },
    { 1 | KCMGL_PROG, GL_MAX_PROGRAM_NATIVE_ALU_INSTRUCTIONS_ARB, "MAX_PROGRAM_NATIVE_ALU_INSTRUCTIONS" },
    { 1 | KCMGL_PROG, GL_MAX_PROGRAM_NATIVE_TEX_INSTRUCTIONS_ARB, "MAX_PROGRAM_NATIVE_TEX_INSTRUCTIONS" },
    { 1 | KCMGL_PROG, GL_MAX_PROGRAM_NATIVE_TEX_INDIRECTIONS_ARB, "MAX_PROGRAM_NATIVE_TEX_INDIRECTIONS" },
    { 1 | KCMGL_PROG, GL_MAX_PROGRAM_NATIVE_TEMPORARIES_ARB, "MAX_PROGRAM_NATIVE_TEMPORARIES" },
    { 1 | KCMGL_PROG, GL_MAX_PROGRAM_NATIVE_PARAMETERS_ARB, "MAX_PROGRAM_NATIVE_PARAMETERS" },
    { 1 | KCMGL_PROG, GL_MAX_PROGRAM_NATIVE_ATTRIBS_ARB, "MAX_PROGRAM_NATIVE_ATTRIBS" },
    {0,0,QString()}
   };
#endif

#ifdef GL_ARB_vertex_program
   const struct token_name arb_vp[] = {
{ 1 | KCMGL_PROG, GL_MAX_PROGRAM_ENV_PARAMETERS_ARB,"MAX_PROGRAM_ENV_PARAMETERS"},
{ 1 | KCMGL_PROG, GL_MAX_PROGRAM_LOCAL_PARAMETERS_ARB,"MAX_PROGRAM_LOCAL_PARAMETERS"},
{ 1, GL_MAX_VERTEX_ATTRIBS_ARB, "MAX_VERTEX_ATTRIBS"},
{ 1, GL_MAX_PROGRAM_MATRICES_ARB,"MAX_PROGRAM_MATRICES"},
{ 1, GL_MAX_PROGRAM_MATRIX_STACK_DEPTH_ARB,"MAX_PROGRAM_MATRIX_STACK_DEPTH"},
{ 1 | KCMGL_PROG, GL_MAX_PROGRAM_INSTRUCTIONS_ARB,"MAX_PROGRAM_INSTRUCTIONS"},
{ 1 | KCMGL_PROG, GL_MAX_PROGRAM_TEMPORARIES_ARB,"MAX_PROGRAM_TEMPORARIES"},
{ 1 | KCMGL_PROG, GL_MAX_PROGRAM_PARAMETERS_ARB,"MAX_PROGRAM_PARAMETERS"},
{ 1 | KCMGL_PROG, GL_MAX_PROGRAM_ATTRIBS_ARB,"MAX_PROGRAM_ATTRIBS"},
{ 1 | KCMGL_PROG, GL_MAX_PROGRAM_ADDRESS_REGISTERS_ARB,"MAX_PROGRAM_ADDRESS_REGISTERS"},
{ 1 | KCMGL_PROG, GL_MAX_PROGRAM_NATIVE_INSTRUCTIONS_ARB,"MAX_PROGRAM_NATIVE_INSTRUCTIONS"},
{ 1 | KCMGL_PROG, GL_MAX_PROGRAM_NATIVE_TEMPORARIES_ARB,"MAX_PROGRAM_NATIVE_TEMPORARIES"},
{ 1 | KCMGL_PROG, GL_MAX_PROGRAM_NATIVE_PARAMETERS_ARB,"MAX_PROGRAM_NATIVE_PARAMETERS"},
{ 1 | KCMGL_PROG, GL_MAX_PROGRAM_NATIVE_ATTRIBS_ARB,"MAX_PROGRAM_NATIVE_ATTRIBS"},
{ 1 | KCMGL_PROG, GL_MAX_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB ,"MAX_PROGRAM_NATIVE_ADDRESS_REGISTERS"},
{0,0,QString()}
};
#endif

#ifdef GL_ARB_vertex_shader
   const struct token_name arb_vs[] = {
    { 1, GL_MAX_VERTEX_ATTRIBS_ARB,"MAX_VERTEX_ATTRIBS"},
    { 1, GL_MAX_VERTEX_UNIFORM_COMPONENTS_ARB,"MAX_VERTEX_UNIFORM_COMPONENTS"},
    { 1, GL_MAX_VARYING_FLOATS_ARB,"MAX_VARYING_FLOATS"},
    { 1, GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS_ARB,"MAX_COMBINED_TEXTURE_IMAGE_UNITS"},
    { 1, GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS_ARB,"MAX_VERTEX_TEXTURE_IMAGE_UNITS"},
    { 1, GL_MAX_TEXTURE_IMAGE_UNITS_ARB,"MAX_TEXTURE_IMAGE_UNITS"},
    { 1, GL_MAX_TEXTURE_COORDS_ARB,"MAX_TEXTURE_COORDS"},
    {0,0,QString()}
   };
#endif

#ifdef GL_ARB_fragment_shader
   const struct token_name arb_fs[] = {
    { 1, GL_MAX_FRAGMENT_UNIFORM_COMPONENTS_ARB,"MAX_FRAGMENT_UNIFORM_COMPONENTS"},
    { 1, GL_MAX_TEXTURE_IMAGE_UNITS_ARB,"MAX_TEXTURE_IMAGE_UNITS"},
    { 1, GL_MAX_TEXTURE_COORDS_ARB,"MAX_TEXTURE_COORDS"},
    {0,0,QString()}
   };
#endif

   const struct token_name frame_buffer_props[] = {
      { 2, GL_MAX_VIEWPORT_DIMS, 	i18n("Max. viewport dimensions") },
      { 1, GL_SUBPIXEL_BITS, 		i18n("Subpixel bits") },
      { 1, GL_AUX_BUFFERS, 		i18n("Aux. buffers")},
      {0,0,QString()}
    };

   const struct token_group groups[] =
   {
    {KCMGL_SIZE(frame_buffer_props), 0, frame_buffer_props, i18n("Frame buffer properties"), NULL},
    {KCMGL_SIZE(various_limits), 0, texture_limits, i18n("Texturing"), NULL},
    {KCMGL_SIZE(various_limits), 0, various_limits, i18n("Various limits"), NULL},
    {KCMGL_SIZE(float_limits), 0, float_limits, i18n("Points and lines"), NULL},
    {KCMGL_SIZE(stack_depth), 0, stack_depth, i18n("Stack depth limits"), NULL},
#ifdef GL_ARB_vertex_program
    {KCMGL_SIZE(arb_vp), GL_VERTEX_PROGRAM_ARB, arb_vp, "ARB_vertex_program", "GL_ARB_vertex_program"},
#endif
#ifdef GL_ARB_fragment_program
    {KCMGL_SIZE(arb_fp), GL_FRAGMENT_PROGRAM_ARB, arb_fp, "ARB_fragment_program", "GL_ARB_fragment_program"},
#endif
#ifdef GL_ARB_vertex_shader
    {KCMGL_SIZE(arb_vs), 0, arb_vs, "ARB_vertex_shader", "GL_ARB_vertex_shader"},
#endif
#ifdef GL_ARB_fragment_shader
    {KCMGL_SIZE(arb_fs), 0, arb_fs, "ARB_fragment_shader", "GL_ARB_fragment_shader"},
#endif
   };

#if defined(GLX_ARB_get_proc_address) && defined(PFNGLGETPROGRAMIVARBPROC)
   if (getProcAddress && strstr(glExtensions, "GL_ARB_vertex_program"))
   kcm_glGetProgramivARB = (PFNGLGETPROGRAMIVARBPROC) glXGetProcAddressARB((const GLubyte *)"glGetProgramivARB");
#else
   Q_UNUSED(getProcAddress);
#endif

   for (uint i = 0; i<KCMGL_SIZE(groups); i++) {
   	if (groups[i].ext && !strstr(glExtensions, groups[i].ext)) continue;

	if (l2) l2 = newItem(l1, l2, groups[i].descr);
   	   else l2 = newItem(l1, groups[i].descr);
	l3 = nullptr;
   	const struct token_name *cur_token;
	for (cur_token = groups[i].group; cur_token->type; cur_token++) {

   		bool tfloat = cur_token->type & KCMGL_FLOAT;
		int count = KCMGL_COUNT_MASK(cur_token->type);
		GLint max[2]={0,0};
   		GLfloat fmax[2]={0.0,0.0};

#if defined(PFNGLGETPROGRAMIVARBPROC) && defined(GL_ARB_vertex_program)
   		bool tprog = cur_token->type & KCMGL_PROG;
		if (tprog && kcm_glGetProgramivARB)
			kcm_glGetProgramivARB(groups[i].type, cur_token->token, max);
		else
#endif
		if (tfloat) glGetFloatv(cur_token->token, fmax);
   			else glGetIntegerv(cur_token->token, max);

		if (glGetError() == GL_NONE) {
			QString s;
			if (!tfloat && count == 1) s = QString::number(max[0]); else
			if (!tfloat && count == 2) s = QStringLiteral("%1, %2").arg(max[0]).arg(max[1]); else
			if (tfloat && count == 2) s = QStringLiteral("%1 - %2").arg(fmax[0],0,'f',6).arg(fmax[1],0,'f',6); else
			if (tfloat && count == 1) s = QString::number(fmax[0],'f',6);
			if (l3) l3 = newItem(l2, l3, cur_token->name, s);
	 			else l3 = newItem(l2, cur_token->name, s);

		}
	}

     }
}
#endif

static void makeDriItem(const DriInfo &info, QTreeWidgetItem *parent, QTreeWidgetItem *preceding)
{
    preceding = newItem(parent, preceding, i18n("Vendor"), info.vendor);
    preceding = newItem(parent, preceding, i18n("Device"), info.device);
    preceding = newItem(parent, preceding, i18n("Subvendor"), info.subvendor);
    preceding = newItem(parent, preceding, i18n("Revision"), info.rev);
    preceding = newItem(parent, preceding, i18n("Kernel module"), info.module);
}

static QTreeWidgetItem *print_screen_info(QTreeWidgetItem *l1, QTreeWidgetItem *after, const QString &title)
{
   	QTreeWidgetItem *l2 = nullptr, *l3 = nullptr;

   	if (after) {
        l1 = newItem(l1, after, title);
    } else {
        l1 = newItem(l1, title);
    }

    if (IsDirect) {
        const QVector<DriInfo> infos = get_dri_devices();
        if (infos.size() > 0) {
            l2 = newItem(l1, i18np("3D Accelerator", "3D Accelerators", infos.size()));
            l2->setExpanded(true);
            if (infos.size() > 1) {
                for (const auto &info : infos) {
                    l3 = newItem(l2, l3, info.deviceId);
                    l3->setExpanded(true);
                    makeDriItem(info, l3, nullptr);
                }
            } else if (infos.size() == 1) {
                makeDriItem(infos.at(0), l2, nullptr);
            }
		} else {
            l2 = newItem(l1, l2, i18n("3D Accelerator"), i18n("unknown"));
        }
    }
    
    if (l2) {
        l2 = newItem(l1, l2, i18n("Driver"));
    }else{
        l2 = newItem(l1, i18n("Driver"));
    }

    l2->setExpanded(true);

  	l3 = newItem(l2, i18n("Vendor"), gli.glVendor);
    	l3 = newItem(l2, l3, i18n("Renderer"), gli.glRenderer);
#ifdef KCM_ENABLE_OPENGLES
        l3 = newItem(l2, l3, i18n("OpenGL ES version"), gli.glVersion);
#else
        l3 = newItem(l2, l3, i18n("OpenGL version"), gli.glVersion);
#endif

#ifdef KCM_ENABLE_OPENGLES
        l3 = newItem(l2, l3, i18n("OpenGL ES extensions"));
#else
        l3 = newItem(l2, l3, i18n("OpenGL extensions"));
#endif
    	print_extension_list(gli.glExtensions, l3);

#if KCM_HAVE_GLX
        if (QGuiApplication::platformName() == QLatin1String("xcb")) {
            l3 = newItem(l2, l3, i18n("Implementation specific"));
            print_limits(l3, gli.glExtensions, strstr(gli.clientExtensions, "GLX_ARB_get_proc_address") != nullptr);
        }
#endif

        return l1;
}

#if KCM_HAVE_GLX
void print_glx_glu(QTreeWidgetItem *l1, QTreeWidgetItem *l2)
{
   QTreeWidgetItem *l3;

    l2 = newItem(l1, l2, i18n("GLX"));
    l3 = newItem(l2, i18n("server GLX vendor"), gli.serverVendor);
    l3 = newItem(l2, l3, i18n("server GLX version"), gli.serverVersion);
    l3 = newItem(l2, l3, i18n("server GLX extensions"));
    print_extension_list(gli.serverExtensions, l3);

    l3 = newItem(l2, l3, i18n("client GLX vendor") ,gli.clientVendor);
    l3 = newItem(l2, l3, i18n("client GLX version"), gli.clientVersion);
    l3 = newItem(l2, l3, i18n("client GLX extensions"));
    print_extension_list(gli.clientExtensions, l3);
    l3 = newItem(l2, l3, i18n("GLX extensions"));
    print_extension_list(gli.glxExtensions, l3);

    l2 = newItem(l1, l2, i18n("GLU"));
    l3 = newItem(l2, i18n("GLU version"), gli.gluVersion);
    l3 = newItem(l2, l3, i18n("GLU extensions"));
    print_extension_list(gli.gluExtensions, l3);

}
#endif

#if KCM_HAVE_EGL
void print_egl(QTreeWidgetItem *l1, QTreeWidgetItem *l2)
{
   QTreeWidgetItem *l3;

   l2 = newItem(l1, l2, i18n("EGL"));
   l3 = newItem(l2, i18n("EGL Vendor"), gli.eglVendor);
   l3 = newItem(l2, l3, i18n("EGL Version"), gli.eglVersion);
   l3 = newItem(l2, l3, i18n("EGL Extensions"));
   print_extension_list(gli.eglExtensions, l3);
}
#endif

#if KCM_HAVE_GLX
static QTreeWidgetItem *get_gl_info_glx(Display *dpy, int scrnum, Bool allowDirect, QTreeWidgetItem *l1, QTreeWidgetItem *after)
{
   Window win;
   XSetWindowAttributes attr;
   unsigned long mask;
   Window root;
   XVisualInfo *visinfo;
   int width = 100, height = 100;
   QTreeWidgetItem *result = after;

   root = RootWindow(dpy, scrnum);

   const int attribSingle[] = {
      GLX_RGBA,
      GLX_RED_SIZE, 1,
      GLX_GREEN_SIZE, 1,
      GLX_BLUE_SIZE, 1,
      None };
   const int attribDouble[] = {
      GLX_RGBA,
      GLX_RED_SIZE, 1,
      GLX_GREEN_SIZE, 1,
      GLX_BLUE_SIZE, 1,
      GLX_DOUBLEBUFFER,
      None };
   GLXContext ctx;

   visinfo = glXChooseVisual(dpy, scrnum, const_cast<int*>(attribSingle));
   if (!visinfo) {
      visinfo = glXChooseVisual(dpy, scrnum, const_cast<int*>(attribDouble));
      if (!visinfo) {
                   qDebug() << "Error: couldn't find RGB GLX visual\n";
         return result;
      }
   }

   attr.background_pixel = 0;
   attr.border_pixel = 0;
   attr.colormap = XCreateColormap(dpy, root, visinfo->visual, AllocNone);
   attr.event_mask = StructureNotifyMask | ExposureMask;
   mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;
   win = XCreateWindow(dpy, root, 0, 0, width, height,
                        0, visinfo->depth, InputOutput,
                       visinfo->visual, mask, &attr);

   ctx = glXCreateContext( dpy, visinfo, NULL, allowDirect );
   if (!ctx) {
                qDebug() << "Error: glXCreateContext failed\n";
      XDestroyWindow(dpy, win);
      XFree(visinfo);
      return result;
   }

   if (glXMakeCurrent(dpy, win, ctx)) {
      gli.serverVendor = glXQueryServerString(dpy, scrnum, GLX_VENDOR);
      gli.serverVersion = glXQueryServerString(dpy, scrnum, GLX_VERSION);
      gli.serverExtensions = glXQueryServerString(dpy, scrnum, GLX_EXTENSIONS);
      gli.clientVendor = glXGetClientString(dpy, GLX_VENDOR);
      gli.clientVersion = glXGetClientString(dpy, GLX_VERSION);
      gli.clientExtensions = glXGetClientString(dpy, GLX_EXTENSIONS);
      gli.glxExtensions = glXQueryExtensionsString(dpy, scrnum);
      gli.glVendor = (const char *) glGetString(GL_VENDOR);
      gli.glRenderer = (const char *) glGetString(GL_RENDERER);
      gli.glVersion = (const char *) glGetString(GL_VERSION);
      gli.glExtensions = (const char *) glGetString(GL_EXTENSIONS);
      gli.displayName = nullptr;
      gli.gluVersion = (const char *) gluGetString(GLU_VERSION);
      gli.gluExtensions = (const char *) gluGetString(GLU_EXTENSIONS);

      IsDirect = glXIsDirect(dpy, ctx);

      result = print_screen_info(l1, after, IsDirect ? i18n("Direct Rendering (GLX)") : i18n("Indirect Rendering (GLX)"));
   }
   else {
      qDebug() << "Error: glXMakeCurrent failed\n";
   }

   glXMakeCurrent(dpy, GL_NONE, nullptr);
   glXDestroyContext(dpy, ctx);
   XDestroyWindow(dpy, win);
   XFree(visinfo);

   return result;
}
#endif

#if KCM_HAVE_EGL
static QTreeWidgetItem *get_gl_info_egl(Display *dpy, int scrnum, QTreeWidgetItem *l1, QTreeWidgetItem *after)
{
   Window win;
   XSetWindowAttributes attr;
   unsigned long mask;
   Window root;
   XVisualInfo *visinfo;
   int width = 100, height = 100;
   QTreeWidgetItem *result = after;

   root = RootWindow(dpy, scrnum);

   static const EGLint attribs[] = {
      EGL_RED_SIZE, 1,
      EGL_GREEN_SIZE, 1,
      EGL_BLUE_SIZE, 1,
      EGL_DEPTH_SIZE, 1,
      EGL_NONE
   };
   static const EGLint ctx_attribs[] = {
      EGL_CONTEXT_CLIENT_VERSION, 2,
      EGL_NONE
   };

   XVisualInfo visTemplate;
   int num_visuals;
   EGLDisplay egl_dpy;
   EGLSurface surf;
   EGLContext ctx;
   EGLConfig config;
   EGLint num_configs;
   EGLint vid, major, minor;

   egl_dpy = eglGetDisplay(dpy);
   if (!egl_dpy) {
      qDebug() << "Error: eglGetDisplay() failed\n";
      return result;
   }

   if (!eglInitialize(egl_dpy, &major, &minor)) {
      qDebug() << "Error: eglInitialize() failed\n";
      return result;
   }

   if (!eglChooseConfig(egl_dpy, attribs, &config, 1, &num_configs)) {
      qDebug() << "Error: couldn't get an EGL visual config\n";
      return result;
   }

   if (!eglGetConfigAttrib(egl_dpy, config, EGL_NATIVE_VISUAL_ID, &vid)) {
      qDebug() << "Error: eglGetConfigAttrib() failed\n";
      return result;
   }

   visTemplate.visualid = vid;
   visinfo = XGetVisualInfo(dpy, VisualIDMask, &visTemplate, &num_visuals);
   if (!visinfo) {
      qDebug() << "Error: couldn't get X visual\n";
      return result;
   }

   attr.background_pixel = 0;
   attr.border_pixel = 0;
   attr.colormap = XCreateColormap(dpy, root, visinfo->visual, AllocNone);
   attr.event_mask = StructureNotifyMask | ExposureMask;
   mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;
   win = XCreateWindow(dpy, root, 0, 0, width, height,
   			0, visinfo->depth, InputOutput,
		       visinfo->visual, mask, &attr);

#ifdef KCM_ENABLE_OPENGLES
    eglBindAPI(EGL_OPENGL_ES_API);
#else
    eglBindAPI(EGL_OPENGL_API);
#endif
   ctx = eglCreateContext(egl_dpy, config, EGL_NO_CONTEXT, ctx_attribs);
   if (!ctx) {
      qDebug() << "Error: eglCreateContext failed\n";
      XDestroyWindow(dpy, win);
      XFree(visinfo);
      return result;
   }

   surf = eglCreateWindowSurface(egl_dpy, config, win, NULL);
   if (!surf) {
      qDebug() << "Error: eglCreateWindowSurface failed\n";
      eglDestroyContext(egl_dpy, ctx);
      XDestroyWindow(dpy, win);
      XFree(visinfo);
      return result;
   }

   if (eglMakeCurrent(egl_dpy, surf, surf, ctx)) {
      gli.eglVendor = eglQueryString(egl_dpy, EGL_VENDOR);
      gli.eglVersion = eglQueryString(egl_dpy, EGL_VERSION);
      gli.eglExtensions = eglQueryString(egl_dpy, EGL_EXTENSIONS);
      gli.glVendor = (const char *) glGetString(GL_VENDOR);
      gli.glRenderer = (const char *) glGetString(GL_RENDERER);
      gli.glVersion = (const char *) glGetString(GL_VERSION);
      gli.glExtensions = (const char *) glGetString(GL_EXTENSIONS);
      gli.displayName = nullptr;
      result = print_screen_info(l1, after, i18n("Direct Rendering (EGL)"));
   }
   else {
      qDebug() <<"Error: eglMakeCurrent() failed\n";
   }

   eglMakeCurrent(egl_dpy, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
   eglDestroyContext(egl_dpy, ctx);
   eglDestroySurface(egl_dpy, surf);
   XDestroyWindow(dpy, win);
   XFree(visinfo);
   return result;

}

static QTreeWidgetItem *get_gl_info_egl_qt(QTreeWidgetItem *l1, QTreeWidgetItem *after, QSurfaceFormat::OpenGLContextProfile profile, const QString &title)
{
    QTreeWidgetItem *result = after;
    QOffscreenSurface surface;
    surface.create();
    QOpenGLContext context;
    QSurfaceFormat format;
    format.setProfile(profile);
    context.setFormat(format);
    if (!context.create()) {
        qDebug() << "Could not create QOpenGLContext";
        return result;
    }
    if (context.format().profile() != profile) {
        qDebug() << "Could not get requested OpenGL profile, requested" << profile << "got" << context.format().profile();
        return result;
    }

    if (context.makeCurrent(&surface)) {
        EGLDisplay egl_dpy = eglGetCurrentDisplay();
        gli.eglVendor = eglQueryString(egl_dpy, EGL_VENDOR);
        gli.eglVersion = eglQueryString(egl_dpy, EGL_VERSION);
        gli.eglExtensions = eglQueryString(egl_dpy, EGL_EXTENSIONS);
        gli.glVendor = (const char *) glGetString(GL_VENDOR);
        gli.glRenderer = (const char *) glGetString(GL_RENDERER);
        gli.glVersion = (const char *) glGetString(GL_VERSION);
        gli.glExtensions = (const char *) glGetString(GL_EXTENSIONS);
        gli.displayName = nullptr;
        result = print_screen_info(l1, after, title);
    }
    else {
        qDebug() <<"Error: eglMakeCurrent() failed\n";
    }

    context.doneCurrent();

    return result;
}
#endif

bool GetInfo_OpenGL(QTreeWidget *treeWidget)
{
    QTreeWidgetItem *l1, *l2 = nullptr;

    static bool isX11 = QGuiApplication::platformName() == QLatin1String("xcb");
    static bool isWayland = QGuiApplication::platformName().contains(QLatin1String("wayland"));

    QTreeWidgetItem *header = new QTreeWidgetItem();
    header->setText(0, i18n("Information"));
    header->setText(1, i18n("Value"));
    treeWidget->setHeaderItem(header);

    treeWidget->setRootIsDecorated(false);

    l1 = new QTreeWidgetItem(treeWidget);

    if (isX11) {
        char *displayName = nullptr;
        Display *dpy;
        int numScreens, scrnum;

        dpy = XOpenDisplay(displayName);
        if (!dpy) {
    //      qDebug() << "Error: unable to open display " << displayName;
            return false;
        }
        l1->setText(0, i18n("Name of the Display"));
        l1->setText(1, QString::fromLatin1(DisplayString(dpy)));
        l1->setExpanded(true);
        l1->setFlags(Qt::ItemIsEnabled);

        numScreens = ScreenCount(dpy);

        scrnum = 0;
#ifdef KCMGL_MANY_SCREENS
        for (; scrnum < numScreens; scrnum++)
#endif
        {
#if KCM_HAVE_GLX
            mesa_hack(dpy, scrnum);

            l2 = get_gl_info_glx(dpy, scrnum, true, l1, l2);
            if (l2) l2->setExpanded(true);
#endif
#if KCM_HAVE_EGL
            l2 = get_gl_info_egl(dpy, scrnum, l1, l2);
            if (l2)
                l2->setExpanded(true);
#endif

//   TODO      print_visual_info(dpy, scrnum, mode);
        }

#if KCM_HAVE_GLX
        if (l2)
            print_glx_glu(l1, l2);
        else
            KMessageBox::error(0, i18n("Could not initialize OpenGL/GLX"));
#endif
        XCloseDisplay(dpy);
    }

    if (isWayland) {
        IsDirect = true;
        l1->setText(0, i18n("Name of the Display"));
        l1->setText(1, QString::fromLatin1(qgetenv("WAYLAND_DISPLAY")));
        l1->setExpanded(true);
        l1->setFlags(Qt::ItemIsEnabled);
#if KCM_HAVE_EGL
        l2 = get_gl_info_egl_qt(l1, l2, QSurfaceFormat::NoProfile, i18n("OpenGL"));
        if (l2)
            l2->setExpanded(true);
        l2 = get_gl_info_egl_qt(l1, l2, QSurfaceFormat::CoreProfile, i18n("Core Profile"));
        if (l2)
            l2->setExpanded(true);
        l2 = get_gl_info_egl_qt(l1, l2, QSurfaceFormat::CompatibilityProfile, i18n("Compatibility Profile"));
        if (l2)
            l2->setExpanded(true);
#endif
    }

#if KCM_HAVE_EGL
    if (l2)
        print_egl(l1, l2);
    else
        KMessageBox::error(0, i18n("Could not initialize OpenGL (ES)/EGL "));
#endif

    treeWidget->resizeColumnToContents(0);
    treeWidget->resizeColumnToContents(1);

    return true;
}

#include "opengl.moc"
