QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    DrawingArea.cpp \
    ColorPopupWidget.cpp \
    ShapeLibraryWidget.cpp \
    PropertyPanel.cpp

HEADERS += \
    mainwindow.h \
    DrawingArea.h \
    ColorPopupWidget.h \
    ShapeLibraryWidget.h \
    PropertyPanel.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# 添加翻译文件
TRANSLATIONS = translations/mypaint_zh_CN.ts

# 创建更新翻译文件的target
update_translations.commands = lupdate $$PWD -ts $$TRANSLATIONS
QMAKE_EXTRA_TARGETS += update_translations

# 创建编译翻译文件的target
QMAKE_PRE_LINK += lrelease $$TRANSLATIONS

# 将qm文件添加到资源
RESOURCES += resources.qrc \
    styles.qrc
