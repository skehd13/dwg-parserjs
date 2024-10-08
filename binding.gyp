{
  "targets": [
    {
      "target_name": "reDwg",
      'xcode_settings': {
        'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
      },
      'include_dirs': [
        '<!@(node -p "require(\'node-addon-api\').include")',
        'include'
      ],
      'dependencies': ["<!(node -p \"require('node-addon-api').gyp\")"],
      'defines': ["NAPI_DISABLE_CPP_EXCEPTIONS"],
      'conditions': [
        ['OS == "mac"', {
          "sources": [ "libreDwgOsx.cpp" ],
          'include_dirs': [
            'System/Library/Frameworks/ApplicationServices.framework/Headers',
            "<!@(pkg-config --cflags libredwg)"
          ],
          "libraries": [
            "<!@(pkg-config --libs libredwg)"
          ],
          'link_settings': {
            'libraries': [
              '-framework ApplicationServices',
            ]
          }
        }],
        ['OS == "win"', {
          "sources": [ "libreDwgWin.cpp" ],
          "include_dirs": [
            "%ProgramFiles(x86)%/libredwg/include",
            "%ProgramFiles%/libredwg/include",
            "%ProgramFiles(x86)%/libredwg/bin",
            "%ProgramFiles%/libredwg/bin"
          ],
          "libraries": [
            "%ProgramFiles(x86)%/libredwg/lib/libredwg.lib",
            "%ProgramFiles%/libredwg/lib/libredwg.lib",
          ],
        }],
      ],
    }
  ]
}