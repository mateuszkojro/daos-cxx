cmake -B build \
      -DWITH_ZIPKIN=ON \
      -DBUILD_TESTING=OFF \
      -DWITH_EXAMPLES=OFF \
      -DDAOS_DIR=./lib/daos/install \
      -DCMAKE_EXPORT_COMPILE_COMMANDS=TRUE \
      -DWITH_OPENTELEMETRY=TRUE \
      -DWITH_STL=TRUE
