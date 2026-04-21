cmake_minimum_required(VERSION 3.19)
project(SideProject_Account LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Charts Network)

qt_standard_project_setup()

qt_add_executable(SideProject_Account
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    account.cpp
    account.h
    transaction.cpp
    transaction.h
    bankmanager.cpp
    bankmanager.h
    sync_client.cpp
    sync_client.h
    sync_server.cpp
    sync_server.h
    sync_state.cpp
    sync_state.h
    Acc_search.ui
    add_acc.ui
    delete_acc.ui
    AccSearchDialog.h
    AccSearchDialog.cpp
    deposit.ui
    withdraw.ui
    DepositDialog.h
    DepositDialog.cpp
    WithdrawDialog.h
    WithdrawDialog.cpp
    AddAccDialog.h
    AddAccDialog.cpp
    DeleteAccDialog.h
    DeleteAccDialog.cpp
    transfer.ui
    TransferDialog.h
    TransferDialog.cpp
    resources.qrc


)

target_link_libraries(SideProject_Account
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Charts
        Qt::Network
)

install(TARGETS SideProject_Account
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET SideProject_Account
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
