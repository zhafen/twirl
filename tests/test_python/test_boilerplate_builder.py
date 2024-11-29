import unittest
import sys
import os
from src.python.boilerplate_builder import get_struct_str

class TestBoilerplateBuilder(unittest.TestCase):
    def test_get_struct_str_empty_fields(self):
        name = "MyStruct"
        fields = {}
        expected_output = "struct MyStruct {};"
        self.assertEqual(get_struct_str(name, fields), expected_output)

    def test_get_struct_str_single_field(self):
        name = "MyStruct"
        fields = {"field1": "int"}
        expected_output = "struct MyStruct {\n    int field1;\n};"
        self.assertEqual(get_struct_str(name, fields), expected_output)

    def test_get_struct_str_multiple_fields(self):
        name = "MyStruct"
        fields = {"field1": "int", "field2": "float", "field3": "std::string"}
        expected_output = (
            "struct MyStruct {\n"
            "    int field1;\n"
            "    float field2;\n"
            "    std::string field3;\n"
            "};"
        )
        self.assertEqual(get_struct_str(name, fields), expected_output)