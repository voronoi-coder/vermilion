# echo $(sed -n '$s/add_executable.\([A-Za-z]*\).*/\1/p' CMakeLists.txt)
sed -n '$s/target_link_libraries.\([A-Za-z]*\).*/\1/p' CMakeLists.txt

