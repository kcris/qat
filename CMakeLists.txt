cmake_minimum_required(VERSION 2.8)

project(qat)

# Tell CMake to run moc when necessary:
#set(CMAKE_AUTOMOC ON)

# As moc files are generated in the binary dir, tell CMake to always look for includes there:
#set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Widgets finds its own dependencies.
#find_package(Qt5Widgets REQUIRED)

#add_executable(qxAdm main.cpp)

#qt5_use_modules(qxAdm Widgets)

#see http://www.kdab.com/using-cmake-with-qt-5/
#see http://doc-snapshot.qt-project.org/5.0/qtdoc/cmake-manual.html
#http://qt-project.org/quarterly/view/using_cmake_to_build_qt_projects


FIND_PACKAGE(Qt4 REQUIRED)

SET(qat_SOURCES 
    src/main.cpp 
    src/Catalog.cpp
    src/TreeModel.cpp
    src/ui/mainwindow.cpp
    src/ui/OptionsDialog.cpp)

SET(qat_HEADERS 
    src/Catalog.h
    src/TreeModel.h
    src/TreeModelItem.h
    src/ui/mainwindow.h
    src/ui/OptionsDialog.h)

SET(qat_FORMS 
    src/ui/mainwindow.ui
    src/ui/OptionsDialog.ui)
    
#SET(qat_RESOURCES qat.qrc)

QT4_WRAP_CPP(qat_HEADERS_MOC ${qat_HEADERS})
QT4_WRAP_UI(qat_FORMS_HEADERS ${qat_FORMS})
#QT4_ADD_RESOURCES(qat_RESOURCES_RCC ${qat_RESOURCES})

INCLUDE_DIRECTORIES(src)

INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

#LINK_DIRECTORIES(../deps/qjson/lib/)

ADD_EXECUTABLE(qat ${qat_SOURCES} ${qat_HEADERS_MOC} ${qat_FORMS_HEADERS} ${qat_RESOURCES_RCC})
TARGET_LINK_LIBRARIES(qat ${QT_LIBRARIES})
