<!--TODO explicar contenidos-->

# M2OS / Zephyr Test 

Este repositorio almacena el código desarrollado para las pruebas de rendimiento de M2OS y Zephyr, así como librerías adicionales que se han utilizado en el proceso de desarrollo.

## Contenidos

- `m2os_tests`
  - `test_name.c`: fichero de código fuente que contiene el test.
- `zephyr_tests`
  - `test_name_dir/`: directorio individual por cada test
  - `headers.h`: fichero de cabecera que almacena las referencias a las librerías empleadas por los tests.
  - `lib_measurements_copy.h` : copia de la librería de mediciones de Zephyr, para compilar los tests, debe copiarse en `zephyr/include/zephyr/posix/`
  - `tests_reports.h`: librería que contiene funciones relacionadas con la comprobación de resultados de los tests. 

