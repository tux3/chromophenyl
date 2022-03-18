set(kcm_SRCS
    ChromophenylKCM.cpp
    plugin.cpp
)

kconfig_add_kcfg_files(kcm_SRCS
    ../ChromophenylConfig.kcfgc
)

qt5_wrap_ui(kcm_SRCS ChromophenylKCM.ui)

qt5_add_dbus_interface(kcm_SRCS ${KWIN_EFFECTS_INTERFACE} kwineffects_interface)

add_library(kwin_chromophenyl_config MODULE ${kcm_SRCS})

target_link_libraries(kwin_chromophenyl_config
    Qt5::Core
    Qt5::DBus
    Qt5::Gui
    KF5::ConfigCore
    KF5::ConfigGui
    KF5::ConfigWidgets
)

install(
    TARGETS
        kwin_chromophenyl_config

    DESTINATION
        ${PLUGIN_INSTALL_DIR}/kwin/effects/configs
)
