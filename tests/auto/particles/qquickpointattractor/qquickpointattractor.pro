CONFIG += testcase
TARGET = tst_qquickpointattractor
SOURCES += tst_qquickpointattractor.cpp
macx:CONFIG -= app_bundle

include (../../shared/util.pri)
TESTDATA = data/*

QT += core-private gui-private v8-private qml-private quick-private quickparticles-private testlib

