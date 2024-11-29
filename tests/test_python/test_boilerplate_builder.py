import unittest
from src.python.boilerplate_builder import BoilerplateBuilder

class TestBoilerplateBuilder(unittest.TestCase):

    def setUp(self):
        self.bb = BoilerplateBuilder()

    def test_get_struct_str_empty_fields(self):
        name = "MyStruct"
        fields = {}
        expected_output = "struct MyStruct {};"
        self.assertEqual(self.bb.get_struct_str(name, fields), expected_output)

    def test_get_struct_str_single_field(self):
        name = "MyStruct"
        fields = {"field1": "int"}
        expected_output = "struct MyStruct {\n    int field1;\n};"
        self.assertEqual(self.bb.get_struct_str(name, fields), expected_output)

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
        self.assertEqual(self.bb.get_struct_str(name, fields), expected_output)