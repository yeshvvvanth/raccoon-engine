bl_info = {
    "name":         "FML Format",
    "author":       "yeshwanth",
    "blender":      (2,7,0),
    "version":      (0,0,1),
    "location":     "File > Import-Export",
    "description":  "Export custom data format FML",
    "category":     "Import-Export"
}
        
import bpy
from bpy_extras.io_utils import ExportHelper

class ExportMyFormat(bpy.types.Operator, ExportHelper):
    bl_idname       = "export_my_format.fml";
    bl_label        = "Export FML";
    bl_options      = {'PRESET'};
    
    filename_ext    = ".FML";
    
    shade_flat = bpy.props.BoolProperty(name="Flat Shading",description="turns on flat shading",default=False)
    def draw(self, context):
        layout = self.layout
        self.layout.prop(self, "shade_flat")
        
    def execute(self, context):
        from . import exporter
        exporter.saveMeshes(self)
        return {'FINISHED'};

def menu_func(self, context):
    self.layout.operator(ExportMyFormat.bl_idname, text="FML Format(.FML)");

def register():
    print("registered bitches!!!!!!!!!!!!! \n")
    bpy.utils.register_module(__name__);
    bpy.types.INFO_MT_file_export.append(menu_func);
    
def unregister():
    bpy.utils.unregister_module(__name__);
    bpy.types.INFO_MT_file_export.remove(menu_func);

if __name__ == "__main__":
    register()
