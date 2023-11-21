Compile to C
protoc --c_out=. authentication.proto

Compile to JS
npx pbjs -t static-module -w commonjs -o authentication.js authentication.proto