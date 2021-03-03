/*
 * Copyright (C) 2017 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     rekols <rekols@foxmail.com>
 * Maintainer: rekols <rekols@foxmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "utils.h"

#include <QFontDatabase>
#include <QImageReader>

QHash<QString, QPixmap> Utils::m_imgCacheHash;
QHash<QString, QString> Utils::m_fontNameCache;

const char kLauncherService[] = "com.deepin.dde.daemon.Launcher";
const char kLauncherIface[] = "/com/deepin/dde/daemon/Launcher";

namespace {
/**
 * 从应用程序桌面文件获取帮助手册id。
 * @param desktop_file 桌面文件的绝对路径
 * @return 返回帮助手册id如果存在,否则返回空字符串。
 */
//QString GetDeepinManualId(const QString &desktop_file)
//{
//    QSettings settings(desktop_file, QSettings::IniFormat);
//    settings.beginGroup("Desktop Entry");
//    const QVariant value = settings.value("X-Deepin-ManualID");
//    if (value.isValid()) {
//        return value.toString();
//    }
//    return "";
//}

} // namespace

//标题映射表
const int langCount = 3;
QString languageArr[][langCount] = {
    //dde model
    {"controlcenter", "控制中心", "Control Center"},
    {"accounts", "帐户设置", "Accounts"},
    {"cloudsync", "Union ID", "Union ID"},
    {"display", "显示设置", "Display Settings"},
    {"defapp", "默认程序设置", "Default Application Settings"},
    {"personalization", "个性化设置", "Personalization Settings"},
    {"network", "网络设置", "Network Settings"},
    {"notification", "通知设置", "Notification Settings"},
    {"sound", "声音设置", "Sound Settings"},
    {"bluetooth", "蓝牙设置", "Bluetooth Settings"},
    {"datetime", "时间日期", "Date and Time"},
    {"power", "电源管理", "Power Management"},
    {"mouse", "鼠标和触控板", "Mouse and Touchpad"},
    {"tablet", "数位板", "Drawing Tablet"},
    {"keyboard", "键盘和语言", "Keyboard and Language"},
    {"voice", "辅助功能", "Assistive Tools"},
    {"update", "系统更新", "Update Settings"},
    {"systeminfo", "系统信息", "System Info"},
    {"License activator", "授权管理", "Authorization Management"},
    {"commoninfo", "通用设置", "General Settings"}
};

//const QMap<Dtk::Core::DSysInfo::UosEdition,QString> map;
//map
//={(Dtk::Core::DSysInfo::UosProfessional,"p")};


struct ReplyStruct {
    QString m_desktop;
    QString m_name;
    QString m_key;
    QString m_iconKey;

    qint64 m_categoryId;
    qint64 m_installedTime;
};

Q_DECLARE_METATYPE(ReplyStruct)

/**
 * @brief operator <<
 * @param argument
 * @param info
 * @return
 */
QDBusArgument &operator<<(QDBusArgument &argument, const ReplyStruct &info)
{
    argument.beginStructure();
    argument << info.m_desktop << info.m_name << info.m_key << info.m_iconKey;
    argument << info.m_categoryId << info.m_installedTime;
    argument.endStructure();
    return argument;
}

/**
 * @brief operator >> //结构体数据检查
 * @param argument
 * @param info
 * @return
 */
const QDBusArgument &operator>>(const QDBusArgument &argument, ReplyStruct &info)
{
    argument.beginStructure();
    argument >> info.m_desktop >> info.m_name >> info.m_key >> info.m_iconKey;
    argument >> info.m_categoryId >> info.m_installedTime;
    argument.endStructure();
    return argument;
}

Utils::Utils(QObject *parent)
    : QObject(parent)
{
}

Utils::~Utils()
{
}

///**
// * @brief Utils::getTime
// * @return
// * 获取系统时间
// */
//struct timeval Utils::getTime()
//{
//    struct timeval tp;
//    gettimeofday(&tp, nullptr);
//    return tp;
//}

///**
// * @brief Utils::showDiffTime
// * @param tpStart 开始计时时间
// * @return
// * 获取时间差
// */
//struct timeval Utils::showDiffTime(struct timeval tpStart)
//{
//    struct timeval tpEnd;
//    gettimeofday(&tpEnd, nullptr);
//    double timeuse =
//        (1000000 * (tpEnd.tv_sec - tpStart.tv_sec) + tpEnd.tv_usec - tpStart.tv_usec) / 1000000.0;
//    qDebug() << __FUNCTION__ << __LINE__ << timeuse << endl;

//    return tpEnd;
//}

///**
// * @brief Utils::getQssContent
// * @param filePath 文件路径
// * @return
// * 读取qss文件内容
// */
//QString Utils::getQssContent(const QString &filePath)
//{
//    QFile file(filePath);
//    QString qss;

//    if (file.open(QIODevice::ReadOnly)) {
//        qss = file.readAll();
//    }

//    return qss;
//}

///**
// * @brief Utils::isFontMimeType
// * @param filePath
// * @return
// * 判断是否为字体文件
// */
//bool Utils::isFontMimeType(const QString &filePath)
//{
//    const QString mimeName = QMimeDatabase().mimeTypeForFile(filePath).name();
//    ;

//    if (mimeName.startsWith("font/") || mimeName.startsWith("application/x-font")) {
//        return true;
//    }

//    return false;
//}

///**
// * @brief Utils::suffixList
// * @return 返回字体文件格式
// */
//QString Utils::suffixList()
//{
//    return QString("Font Files (*.ttf *.ttc *.otf)");
//}

/**
 * @brief Utils::renderSVG
 * @param filePath 文件路径
 * @param size 图标大小
 * @return
 * 根据传入的路径，大小，应用信息得到像素图
 */
QPixmap Utils::renderSVG(const QString &filePath, const QSize &size)
{
    if (m_imgCacheHash.contains(filePath)) {
        return m_imgCacheHash.value(filePath);
    }

    QImageReader reader;
    QPixmap pixmap;

    reader.setFileName(filePath);

    if (reader.canRead()) {
        const qreal ratio = qApp->devicePixelRatio();
        reader.setScaledSize(size * ratio);
        pixmap = QPixmap::fromImage(reader.read());
        pixmap.setDevicePixelRatio(ratio);
    } else {
        pixmap.load(filePath);
    }

    m_imgCacheHash.insert(filePath, pixmap);

    return pixmap;
}

//QString Utils::loadFontFamilyByType(FontType fontType)
//{
//    QString fontFileName = "";
//    switch (fontType) {
//    case SourceHanSansMedium:
//        fontFileName = ":/font/SourceHanSansCN-Medium.ttf";
//        break;
//    case SourceHanSansNormal:
//        fontFileName = ":/font/SourceHanSansCN-Normal.ttf";
//        break;
//    case DefautFont:
//        QFont font;
//        return font.family();
//    }

//    if (m_fontNameCache.contains(fontFileName)) {
//        return m_fontNameCache.value(fontFileName);
//    }

//    QString fontFamilyName = "";
//    QFile fontFile(fontFileName);
//    if (!fontFile.open(QIODevice::ReadOnly)) {
//        qDebug() << "Open font file error";
//        return fontFamilyName;
//    }

//    int loadedFontID = QFontDatabase::addApplicationFontFromData(fontFile.readAll());
//    QStringList loadedFontFamilies = QFontDatabase::applicationFontFamilies(loadedFontID);
//    if (!loadedFontFamilies.empty()) {
//        fontFamilyName = loadedFontFamilies.at(0);
//    }
//    fontFile.close();

//    m_fontNameCache.insert(fontFileName, fontFamilyName);
//    return fontFamilyName;
//}

///**
// * @brief Utils::loadFontBySizeAndWeight
// * @param fontFamily 字体格式
// * @param fontSize 字体大小
// * @param fontWeight 粗细
// * @return
// * 设置传入字体的大小和粗细
// */
//QFont Utils::loadFontBySizeAndWeight(QString fontFamily, int fontSize, int fontWeight)
//{
//    QFont font(fontFamily);
//    font.setPixelSize(fontSize);
//    font.setWeight(fontWeight);

//    return font;
//}

///**
// * @brief Utils::fromSpecialEncoding
// * @param inputStr
// * @return 返回文本的 utf-8 格式
// * 把文本转成utf-8编码格式
// */
//QString Utils::fromSpecialEncoding(const QString &inputStr)
//{
//    qDebug() << "inputStr is:" << inputStr << endl;
//    bool bFlag = inputStr.contains(QRegExp("[\\x4e00-\\x9fa5]+"));
//    if (bFlag) {
//        return inputStr;
//    }

//    QTextCodec *codec = QTextCodec::codecForName("utf-8");
//    if (codec) {
//        QString unicodeStr = codec->toUnicode(inputStr.toLatin1());
//        qDebug() << "convert to unicode:" << unicodeStr << endl;
//        return unicodeStr;
//    } else {
//        return inputStr;
//    }
//}

/**
 * @brief Utils::translateTitle 返回title映射字段，目前主要用于＂控制中心＂跳转
 * @param titleUS
 * @return
 * @note 除简体中文外,其他暂时都当英文.
 */
QString Utils::translateTitle(const QString &titleUS)
{
    QString strRet = titleUS;
    int nCount = sizeof(languageArr) / sizeof(languageArr[0]);
    for (int i = 0; i < nCount; i++) {
        if (languageArr[i][0] == titleUS) {
            if (QLocale::system().name() == "zh_CN" || QLocale::system().name() == "zh_HK" || QLocale::system().name() == "zh_TW") {
                strRet = languageArr[i][1];
            } else {
                strRet = languageArr[i][2];
            }
        }
    }
    return strRet;
}

/**
 * @brief Utils::launcherInterface
 * @return 返回系统所有应用列表
 */
QList<AppInfo> Utils::launcherInterface()
{
    QList<AppInfo> applist;

    qRegisterMetaType<ReplyStruct>("ReplyStruct");
    qDBusRegisterMetaType<ReplyStruct>();
    qRegisterMetaType<QList<ReplyStruct>>("a");
    qDBusRegisterMetaType<QList<ReplyStruct>>();

    QDBusInterface iface(kLauncherService,
                         kLauncherIface,
                         kLauncherService,
                         QDBusConnection::sessionBus());
    //root权限下此dbus接口无效...
    if (!iface.isValid()) {
        qDebug() << qPrintable(QDBusConnection::sessionBus().lastError().message());
        return applist;
//        exit(1);
    }

    QDBusReply<QList<ReplyStruct>> reply = iface.callWithArgumentList(QDBus::CallMode::AutoDetect, "GetAllItemInfos", QVariantList());
    qDebug() << reply.error().message();

    if (reply.isValid()) {
        QList<ReplyStruct> list;
        list = reply.value();

        for (int var = 0; var < list.size(); ++var) {
            AppInfo app;
            app.key = list.at(var).m_key;
            app.name = list.at(var).m_name;
            app.desktop = list.at(var).m_desktop;
            app.category_id = list.at(var).m_categoryId;
            app.installed_time = list.at(var).m_installedTime;
            applist.append(app);
            //qDebug() << "dbusMsg ---- : " << var << list.at(var).m_name;
        }
//        qDebug() << applist.size() << "applist " <<  applist;
        return applist;
    } else {
        qDebug() << "GetAllItemInfos fail! " << reply.error().message();
        return applist;
    }
}

/**
 * @brief Utils::getSystemManualList
 * @return　返回系统中存在帮助手册的应用列表
 */
QStringList Utils::getSystemManualList()
{
    const QHash<QString, QString> kAppNameMap = {
        {"org.deepin.flatdeb.deepin-calendar", "dde-calendar"},
        {"org.deepin.flatdeb.deepin-music", "deepin-music"},
        {"org.deepin.flatdeb.deepin-screenshot", "deepin-screenshot"},
        {"org.deepin.flatdeb.deepin-voice-recorder", "deepin-voice-recorder"},
        {"deepin-cloud-print-configurator", "deepin-cloud-print"},
        {"org.deepin.flatdeb.deepin-image-viewer", "deepin-image-viewer"},
        {"deepin-cloud-scan-configurator", "deepin-cloud-scan"},
        {"org.deepin.flatdeb.deepin-movie", "deepin-movie"},
        {"org.deepin.flatdeb.deepin-screen-recorder", "deepin-screen-recorder"},
        {"org.deepin.flatdeb.deepin-calculator", "deepin-calculator"},
        {"com.deepin.editor", "deepin-editor"},
        {"org.deepin.scaner", "scaner"},
    };

    QStringList app_list_;
    const AppInfoList list = launcherInterface();
    const QStringList applicationList = QDir(QString("%1/application/").arg(DMAN_MANUAL_DIR)).entryList();
    const QStringList systemList = QDir(QString("%1/system/").arg(DMAN_MANUAL_DIR)).entryList();

#if 1 //旧文案结构兼容
    QString oldMdPath = DMAN_MANUAL_DIR;
    if (Dtk::Core::DSysInfo::DeepinServer == Dtk::Core::DSysInfo::deepinType()) {
        oldMdPath += "/server";
    } else if (Dtk::Core::DSysInfo::DeepinPersonal == Dtk::Core::DSysInfo::deepinType()) {
        oldMdPath += "/personal";
    } else {
        if (Dtk::Core::DSysInfo::isCommunityEdition()) {
            oldMdPath += "/community";
        } else {
            oldMdPath += "/professional";
        }
    }
    const QStringList oldAppList = QDir(oldMdPath).entryList();
#endif

    QMultiMap<qlonglong, AppInfo> appMap;
    for (int var = 0; var < list.size(); ++var) {
        appMap.insert(list.at(var).installed_time, list.at(var));
    }
    //安装时间相同时,按名称排序
    QList<AppInfo> listApp = sortAppList(appMap);

    for (int i = 0; i < listApp.size(); ++i) {
        const QString app_name = kAppNameMap.value(listApp.at(i).key, listApp.at(i).key);
        if ((applicationList.contains(app_name) || oldAppList.contains(app_name))  && app_list_.indexOf(app_name) == -1) {
            app_list_.append(app_name);
        }
    }

    if (systemList.contains("dde") || oldAppList.contains("dde")) {
        app_list_.append("dde");
    }

    qDebug() << "exist app list: " << app_list_ << ", count:" << app_list_.size();
    return app_list_;
}

/**
 * @brief Utils::getSystemManualDir
 * @return
 * @note　获取系统版本信息
 */
QString Utils::getSystemManualDir()
{
    QString strMANUAL_DIR = DMAN_MANUAL_DIR;
//    int nType = Dtk::Core::DSysInfo::deepinType();
//    if (Dtk::Core::DSysInfo::DeepinServer == (Dtk::Core::DSysInfo::DeepinType)nType) {
//        strMANUAL_DIR += "/server";
//    } else if (Dtk::Core::DSysInfo::DeepinPersonal == (Dtk::Core::DSysInfo::DeepinType)nType) {
//        strMANUAL_DIR += "/personal";
//    } else {
//        if (Dtk::Core::DSysInfo::isCommunityEdition()) {
//            strMANUAL_DIR += "/community";
//        } else {
//            strMANUAL_DIR += "/professional";
//        }
//    }
    return strMANUAL_DIR;
}

/**
 * @brief Utils::sortAppList
 * @param map
 * @return 返回排序后的ａｐｐｌｉｓｔ
 * @note appList　排序,如果安装时间相同时按字母前后排序
 */
QList<AppInfo> Utils::sortAppList(QMultiMap<qlonglong, AppInfo> map)
{
    QMapIterator<qlonglong, AppInfo> it(map);
    QList<AppInfo> listEnd;
    QList<AppInfo> listtmp;
    qlonglong longlongtmp = 0;
    while (it.hasNext()) {
        it.next();
        //只在第一次循环时插入listtemp
        if (it.value().key == map.first().key) {
            listtmp.append(it.value());
            longlongtmp = it.key();
            continue;
        }

        //如果这本次key与longlongtemp相等说明，当前应用的安装时间与上一次循环中的应用安装时间相同，把appInfo插入listtemp等待排序
        //如果不相等，listtemp不为null，则对littemp按“应用包名”进行排序；
        //并把排序结果添加到listEnd;
        //清空listtmp, 修改longlongtmp记录当前key;
        if (it.key() == longlongtmp) {
            listtmp.append(it.value());
        } else if (listtmp.size() != 0 && it.key() != longlongtmp) {
            AppInfo m;
            for (int i = 0; i < listtmp.size(); ++i) {
                for (int j = 0; j < listtmp.size() - 1; ++j) {
                    if (listtmp.at(j).key > listtmp.at(j + 1).key) {
                        m = listtmp.at(j);
                        listtmp[j] = listtmp[j + 1];
                        listtmp[j + 1] = m;
                    }
                }
            }
            listEnd.append(listtmp);
            listtmp.clear();
            longlongtmp = it.key();
            listtmp.append(it.value());
        }
    }
    //最后判断listtmp是否为空，处理循环结束时，最后几次longlongtmp都是相等的情况
    if (!listtmp.isEmpty()) {
        QList<AppInfo> temp;
        {
            AppInfo m;
            for (int i = 0; i < listtmp.size(); ++i) {
                for (int j = 0; j < listtmp.size() - 1; ++j) {
                    if (listtmp.at(j).key > listtmp.at(j + 1).key) {
                        m = listtmp.at(j);
                        listtmp[j] = listtmp[j + 1];
                        listtmp[j + 1] = m;
                    }
                }
            }
            temp.append(listtmp);
        }
        listEnd.append(temp);
    }
    return listEnd;
}

/**
 * @brief Utils::hasSelperSupport
 * @return
 * @note 判断是否需要显示服务与支持功能
 */
bool Utils::hasSelperSupport()
{
    int nType = Dtk::Core::DSysInfo::deepinType();
    if (Dtk::Core::DSysInfo::DeepinProfessional == (Dtk::Core::DSysInfo::DeepinType)nType) {
        const QStringList list = getSystemManualList();
        if (list.contains("uos-service-support")) {
            return true;
        }
    }
    return false;
}

//p表示桌面专业版，h表示个人版，d表示社区版
//s表示默认服务器版，e表示服务器企业版，eu表示服务器欧拉版，i表示服务器行业版，klu表示KelvinU项目版本，pgv表示PanguV项目版本。
QStringList Utils::systemToOmit(Dtk::Core::DSysInfo::UosEdition type)
{
    QStringList retList;
    switch (type) {
    //专业版
    case  Dtk::Core::DSysInfo::UosProfessional:
        retList.append("p");
        break;
    //个人版
    case  Dtk::Core::DSysInfo::UosHome:
        retList.append("h");
        break;
    //社区版
    case  Dtk::Core::DSysInfo::UosCommunity:
        retList.append("d");
        break;
    //服务器企业版
    case  Dtk::Core::DSysInfo::UosEnterprise:
        retList.append("e");
        retList.append("s");
        break;
    //服务器行业版
    case  Dtk::Core::DSysInfo::UosEnterpriseC:
        retList.append("i");
        retList.append("s");
        break;
    //服务器欧拉版
    case  Dtk::Core::DSysInfo::UosEuler:
        retList.append("eu");
        retList.append("s");
        break;
    default:
        break;
    }
    return retList;
}

/**
 * @brief Utils::isMostPriority 判断当前文件是否为最优先级文件
 * @param mdPath
 * @param morePriorityPath
 * @return
 */
bool Utils::isMostPriority(const QString &mdPath, QString &morePriorityPath)
{
    Q_UNUSED(mdPath);
    Q_UNUSED(morePriorityPath);

    return  true;
}

bool Utils::activeWindow(quintptr winId)
{
    bool bsuccess = true;
    // new interface use applicationName as id
    QDBusInterface manual("com.deepin.dde.daemon.Dock", "/com/deepin/dde/daemon/Dock",
                          "com.deepin.dde.daemon.Dock");
    QDBusReply<void> reply = manual.call("ActivateWindow", winId);
    if (!reply.isValid()) {
        qDebug() << "call com.deepin.dde.daemon.Dock failed" << reply.error();
        bsuccess = false;
    }
    return bsuccess;
}

ExApplicationHelper *ExApplicationHelper::instance()
{
    return qobject_cast<ExApplicationHelper *>(DGuiApplicationHelper::instance());
}

#define CAST_INT static_cast<int>

QColor light_qpalette[QPalette::NColorRoles] {
    QColor(0x00, 0x00, 0x00),                         // WindowText
    QColor(0xe5, 0xe5, 0xe5),                         // Button
    QColor(0xe6, 0xe6, 0xe6),                         // Light
    QColor(0xe5, 0xe5, 0xe5),                         // Midlight
    QColor(0xe3, 0xe3, 0xe3),                         // Dark
    QColor(0xe4, 0xe4, 0xe4),                         // Mid
    QColor(0x41, 0x4d, 0x68),                         // Text
    Qt::black,                                   // BrightText
    QColor(0x41, 0x4d, 0x68),                         // ButtonText
    Qt::white,                                   // Base
    QColor(0xf8, 0xf8, 0xf8),                         // Window
    QColor(0, 0, 0, CAST_INT(0.05 * 255)),       // Shadow
    QColor(0x00, 0x81, 0xff),                           // Highlight
    Qt::white,                                   // HighlightedText
    QColor(0x00, 0x82, 0xfa),                         // Link
    QColor(0xad, 0x45, 0x79),                         // LinkVisited
    QColor(0, 0, 0, CAST_INT(0.03 * 255)),       // AlternateBase
    Qt::white,                                   // NoRole
    QColor(255, 255, 255, CAST_INT(0.8 * 255)),  // ToolTipBase
    Qt::black                                    // ToolTipText
};

QColor dark_qpalette[QPalette::NColorRoles] {
    QColor(0xff, 0xff, 0xff),                      // WindowText
    QColor(0x44, 0x44, 0x44),                      // Button
    QColor(0x48, 0x48, 0x48),                      // Light
    QColor(0x47, 0x47, 0x47),                      // Midlight
    QColor(0x41, 0x41, 0x41),                      // Dark
    QColor(0x43, 0x43, 0x43),                      // Mid
    QColor(0xc0, 0xc6, 0xd4),                      // Text
    Qt::white,                                // BrightText
    QColor(0xc0, 0xc6, 0xd4),                      // ButtonText
    QColor(0x28, 0x28, 0x28),                      // Base
    QColor(0x25, 0x25, 0x25),                      // Window
    QColor(0, 0, 0, CAST_INT(0.05 * 255)),    // Shadow
    QColor(0x00, 0x81, 0xff),                      // Highlight
    QColor(0xF1, 0xF6, 0xFF),                      // HighlightedText
    QColor(0x00, 0x82, 0xfa),                      // Link
    QColor(0xad, 0x45, 0x79),                      // LinkVisited
    QColor(0, 0, 0, CAST_INT(0.05 * 255)),    // AlternateBase
    Qt::black,                                // NoRole
    QColor(45, 45, 45, CAST_INT(0.8 * 255)),  // ToolTipBase
    QColor(0xc0, 0xc6, 0xd4)                       // ToolTipText
};

QColor light_dpalette[DPalette::NColorTypes] {
    QColor(), // NoType
    QColor(255, 255, 255, CAST_INT(0.2 * 255)), // ItemBackground
    //    QColor("#001A2E"),                           // TextTitle
    //    QColor("#8AA1B4"),                           // TextTips
    //    QColor("#FF5736"),                           // TextWarning
    //    QColor("#0082FA"),                           // TextLively
    //    QColor("#25b7ff"),                           // LightLively
    QColor(0x00, 0x1A, 0x2E), // TextTitle
    QColor(0x8A, 0xA1, 0xB4), // TextTips
    QColor(0xFF, 0x57, 0x36), // TextWarning
    QColor(0x00, 0x82, 0xFA), // TextLively
    QColor(0x25, 0xb7, 0xff), // LightLively

    QColor(235, 235, 235, CAST_INT(0 * 255)), // DarkLively
    QColor(235, 235, 235, CAST_INT(0 * 255)) // FrameBorder
};

QColor dark_dpalette[DPalette::NColorTypes] {
    QColor(), // NoType
    QColor(25, 25, 25, CAST_INT(0 * 255)), // ItemBackground
    //    QColor("#C0C6D4"),                       // TextTitle
    //    QColor("#6D7C88"),                       // TextTips
    //    QColor("#FF5736"),                       // TextWarning
    //    QColor("#0082FA"),                       // TextLively
    //    QColor("#0056c1"),                       // LightLively
    QColor(0xC0, 0xC6, 0xD4), // TextTitle
    QColor(0x6D, 0x7C, 0x88), // TextTips
    QColor(0xFF, 0x57, 0x36), // TextWarning
    QColor(0x00, 0x82, 0xFA), // TextLively
    QColor(0x00, 0x56, 0xc1), // LightLively
    QColor(25, 25, 25, CAST_INT(0 * 255)), // DarkLively
    QColor(25, 25, 25, CAST_INT(0.5 * 255)) // FrameBorder
};

DPalette ExApplicationHelper::standardPalette(DGuiApplicationHelper::ColorType type) const
{
    DPalette pa;
    const QColor *qcolor_list, *dcolor_list;

    if (type == DarkType) {
        qcolor_list = dark_qpalette;
        dcolor_list = dark_dpalette;
    } else {
        qcolor_list = light_qpalette;
        dcolor_list = light_dpalette;
    }

    for (int i = 0; i < DPalette::NColorRoles; ++i) {
        QPalette::ColorRole role = static_cast<QPalette::ColorRole>(i);

        QColor color = qcolor_list[i];
        pa.setColor(DPalette::Active, role, color);
        generatePaletteColor(pa, role, type);
    }

    for (int i = 0; i < DPalette::NColorTypes; ++i) {
        DPalette::ColorType role = static_cast<DPalette::ColorType>(i);

        QColor color = dcolor_list[i];
        pa.setColor(DPalette::Active, role, color);
        generatePaletteColor(pa, role, type);
    }

    return pa;
}

DPalette ExApplicationHelper::palette(const QWidget *widget, const QPalette &base) const
{
    Q_UNUSED(base)

    DPalette palette;

    do {
        // 存在自定义palette时应该根据其自定义的palette获取对应色调的DPalette
        const QPalette &wp = widget->palette();

        palette = standardPalette(toColorType(wp));

        // 关注控件palette改变的事件
        const_cast<QWidget *>(widget)->installEventFilter(const_cast<ExApplicationHelper *>(this));
    } while (false);

    return palette;
}

void ExApplicationHelper::setPalette(QWidget *widget, const DPalette &palette)
{
    // 记录此控件被设置过palette
    widget->setProperty("_d_set_palette", true);
    widget->setPalette(palette);
}

ExApplicationHelper::ExApplicationHelper()
{
}

ExApplicationHelper::~ExApplicationHelper()
{
}

bool ExApplicationHelper::eventFilter(QObject *watched, QEvent *event)
{
    return DGuiApplicationHelper::eventFilter(watched, event);
}
