{
  "targets": [
    {
      "target_name": "unidecode",
      "sources": [
        "src/addon.cxx"
      ],
      "include_dirs"  : [
            "<!(node -e \"require('nan')\")"
      ],
      "cflags": ["-g"],
      'cflags_cc!': [ '-fno-exceptions' ],
      'conditions': [
        ['OS=="mac"', {
          'xcode_settings': {
            'GCC_ENABLE_CPP_EXCEPTIONS': 'YES'
          }
        }]
      ]
    }
  ]
}