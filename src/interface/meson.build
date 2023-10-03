project('app', 'cpp',
  version : '1.01',
  default_options : ['warning_level=3', 'cpp_std=c++17'])
app_env = ['-DAPP_ENV="DEBUG"', '-DSPDLOG_LEVEL="debug"', '-DRFID_SERVER_ADDR="192.168.1.201"', '-DRFID_SERVER_PORT=8080']

epoxy = dependency('epoxy')

default_library = 'static'

gnome = import('gnome')
src_dir = meson.current_source_dir()

if epoxy.found()
  app_resources = gnome.compile_resources('app-resources',
    'app.gresource.xml',
    source_dir: join_paths(src_dir, 'assets'),
    # install: false
  )

c_files = [
  'main.cc',
  'LoginWindow.cc',
  'App.cc',
  'BaseWindow.cc',
  'NetworkClient.cc',
  'InputDialog.cc',
  'ControlsWindow.cc'
]

executable('app',
           c_files,
           app_resources,
           dependencies : [
              dependency('gtkmm-4.0'),
              dependency('spdlog'),
              dependency('epoxy')
              ],
           install : true,
           cpp_args: app_env,
            # implicit_include_directories: false,
            # win_subsystem: 'windows',
            # build_by_default: build_demos,
            # link_args: gui_app_ldflags,
            install: false,


           )
endif