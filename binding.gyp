{
  'targets': [
    {
      'target_name': 'stellarsolver-native',
      'sources': [ 'src/ss.cc', 'src/stellarsolver.cc', 'src/structures.cc' ],
      'include_dirs': ["<!@(node -p \"require('node-addon-api').include\")"],
      'cflags_cc': [
         "-std=c++17",
         "-fexceptions"
      ],
     'dependencies': ["<!(node -p \"require('node-addon-api').gyp\")"],
      'conditions': [
        ['OS=="linux"', {
          'cflags': [
            '<!@(pkg-config --cflags Qt5Core Qt5Widgets stellarsolver cfitsio)'
          ],
          'ldflags': [
            '<!@(pkg-config --libs-only-L --libs-only-other Qt5Core stellarsolver cfitsio)'
          ],
          'libraries': [
            '<!@(pkg-config --libs-only-l Qt5Core stellarsolver cfitsio)'
          ]
        }]
      ]
    }
  ]
}
