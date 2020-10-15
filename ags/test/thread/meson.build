static_tests = [
  'ags_destroy_worker_test',
#  'ags_functional_thread_test', TODO: missing header?
  'ags_returnable_thread_test',
  'ags_task_test',
  'ags_thread_pool_test',
  'ags_thread_test',
  'ags_worker_thread_test',
]

static_test_dependencies = [
  common_test_dependencies,
  xml2_dependency,
  soup_dependency,
  m_dependency,
]

foreach static_test : static_tests
  source = ['@0@.c'.format(static_test)]
  if static_test.contains('/')
    tmp = static_test.split('/')
    static_test = '_'.join(tmp)
  endif

  static_test_bin = executable(
    static_test,
    source,
    c_args: [compiler_flags],
    include_directories: [includes],
    dependencies: [static_test_dependencies],
    link_with: [
      liblib,
      libobject,
      libthread,
    ]
  )

  test(static_test, static_test_bin)
endforeach
