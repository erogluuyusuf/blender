import bpy
from bpy.props import *
from .. base import FunctionNode
from .. node_builder import NodeBuilder

def create_variadic_math_node(data_type, idname, label):

    class MathNode(bpy.types.Node, FunctionNode):
        bl_idname = idname
        bl_label = label

        variadic: NodeBuilder.BaseListVariadicProperty()

        def declaration(self, builder: NodeBuilder):
            builder.base_list_variadic_input("inputs", "variadic", data_type)

            if NodeBuilder.BaseListVariadicPropertyHasList(self.variadic):
                builder.fixed_output("result", "Result", data_type + " List")
            else:
                builder.fixed_output("result", "Result", data_type)

    return MathNode

def create_two_inputs_math_node(data_type, idname, label):

    class MathNode(bpy.types.Node, FunctionNode):
        bl_idname = idname
        bl_label = label

        use_list__a: NodeBuilder.VectorizedProperty()
        use_list__b: NodeBuilder.VectorizedProperty()

        def declaration(self, builder: NodeBuilder):
            builder.vectorized_input("a", "use_list__a", "A", "A", data_type)
            builder.vectorized_input("b", "use_list__b", "B", "B", data_type)
            builder.vectorized_output("result", ["use_list__a", "use_list__b"], "Result", "Result", data_type)

    return MathNode

def create_single_input_math_node(data_type, idname, label):

    class MathNode(bpy.types.Node, FunctionNode):
        bl_idname = idname
        bl_label = label

        use_list: NodeBuilder.VectorizedProperty()

        def declaration(self, builder: NodeBuilder):
            builder.vectorized_input("input", "use_list", "Value", "Values", data_type)
            builder.vectorized_output("output", ["use_list"], "Result", "Result", data_type)

    return MathNode

AddFloatsNode = create_variadic_math_node("Float", "fn_AddFloatsNode", "Add Floats")
MultiplyFloatsNode = create_variadic_math_node("Float", "fn_MultiplyFloatsNode", "Multiply Floats")
MinimumFloatsNode = create_variadic_math_node("Float", "fn_MinimumFloatsNode", "Minimum Floats")
MaximumFloatsNode = create_variadic_math_node("Float", "fn_MaximumFloatsNode", "Maximum Floats")

SubtractFloatsNode = create_two_inputs_math_node("Float", "fn_SubtractFloatsNode", "Subtract Floats")
DivideFloatsNode = create_two_inputs_math_node("Float", "fn_DivideFloatsNode", "Divide Floats")
PowerFloatsNode = create_two_inputs_math_node("Float", "fn_PowerFloatsNode", "Power Floats")

SqrtFloatNode = create_single_input_math_node("Float", "fn_SqrtFloatNode", "Sqrt Float")
AbsFloatNode = create_single_input_math_node("Float", "fn_AbsoluteFloatNode", "Absolute Float")
SineNode = create_single_input_math_node("Float", "fn_SineNode", "Sine")
CosineNode = create_single_input_math_node("Float", "fn_CosineNode", "Cosine")
