add_subdirectory(kcm)

set(effect_SRCS
    Chromophenyl.cpp
    chromophenyl.qrc
    plugin.cpp
)

kconfig_add_kcfg_files(effect_SRCS
    ChromophenylConfig.kcfgc
)

add_library(kwin4_effect_chromophenyl SHARED ${effect_SRCS})

target_link_libraries(kwin4_effect_chromophenyl
    Qt5::Core
    Qt5::Gui
    KF5::ConfigCore
    KF5::ConfigGui
    KF5::CoreAddons
    KF5::WindowSystem
    kwineffects::kwineffects
    kwineffects::kwinglutils
    epoxy::epoxy
)

install(
    TARGETS
        kwin4_effect_chromophenyl

    DESTINATION
        ${PLUGIN_INSTALL_DIR}/kwin/effects/plugins/
)
