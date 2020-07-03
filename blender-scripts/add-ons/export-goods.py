bl_info = {
    "name": "Batch Export Flan Goods",
    "blender": (2, 80, 0),
    "location": "File > Export > Batch Export Flan Goods",
    "description": "Batch Export Flan Goods",
    "category": "Import-Export"}

import os, struct, math
import mathutils
import bpy
import bpy_extras.io_utils
import logging

log = logging.getLogger(__name__)

class ExportBatchGoods(bpy.types.Operator):
    '''Batch Export Flan Goods'''
    bl_idname = "export.batchflangoods"
    bl_label = 'Batch Export Flan Goods'
    
    directory: bpy.props.StringProperty(
        name="Outdir Path",
        description="Horse")
    
    onlylist: bpy.props.BoolProperty(
        name="Only List",
        description="Only list files without exporting")

    def invoke(self, context, _event):
        context.window_manager.fileselect_add(self)
        return {'RUNNING_MODAL'}

    def check(self, _context):
        return True

    def execute(self, context):
        oldscene = bpy.context.window.scene
        try:
            objdir = os.path.join(self.properties.directory, 'models', 'goods')
            imagedir = os.path.join(self.properties.directory, 'textures', 'goods')
            
            for scene in bpy.data.scenes:
                if scene.name == 'default_scene':
                    continue
                
                for collection in scene.collection.children:
                    if collection.name == 'Out' or collection.name[:4] == 'Out.':
                        for object in collection.objects:
                            if type(object.data) == bpy.types.Mesh:
                                objpath = os.path.join(objdir, object.name + '.obj')
                                if not self.properties.onlylist:
                                    # Export OBJ
                                    bpy.context.window.scene = scene
                                    oldselected = list(bpy.context.selected_objects)
                                    oldactive = bpy.context.view_layer.objects.active
                                    try:
                                        # Deselect all
                                        for o in list(bpy.context.selected_objects):
                                            bpy.context.view_layer.objects.active = o
                                            bpy.context.object.select_set(False)
                                        # Select obj
                                        bpy.context.view_layer.objects.active = object
                                        bpy.context.object.select_set(True)
                                        # Export
                                        bpy.ops.export_scene.obj(
                                            filepath=objpath,
                                            use_selection=True,
                                            use_edges=False,
                                            use_smooth_groups=False,
                                            use_materials=False,
                                            use_triangles=True)
                                    except:
                                        self.report({'ERROR'}, 'Failed to export object ' + object.name)
                                        raise
                                    finally:
                                        # Deselect all
                                        for o in list(bpy.context.selected_objects):
                                            bpy.context.view_layer.objects.active = o
                                            bpy.context.object.select_set(False)
                                        # Reselect old
                                        for o in oldselected:
                                            bpy.context.view_layer.objects.active = o
                                            bpy.context.object.select_set(True)
                                        #Reset active
                                        bpy.context.view_layer.objects.active = oldactive
                                else:
                                    print('export-goods: ' + objpath)
                                
                                # Try export material
                                if object.name in object.data.materials:
                                    material = object.data.materials[object.name]
                                    if material.name in bpy.data.images:
                                        img = bpy.data.images[material.name]
                                        imgpath = os.path.join(imagedir, img.name + '.png')
                                        if not self.properties.onlylist:
                                            oldpath = img.filepath
                                            img.filepath = imgpath
                                            try:
                                                img.save()
                                            except:
                                                self.report({'ERROR'}, 'Failed to save image ' + img.name)
                                                raise
                                            finally:
                                                img.filepath = oldpath
                                        else:
                                            print('export-goods: ' + imgpath)
                                    else:
                                        self.report({'WARNING'}, 'Image not found for material ' + material.name)
                                else:
                                    self.report({'WARNING'}, 'Material not found for object ' + object.name)
                    elif collection.name != 'Collection':
                        self.report({'WARNING'}, 'Non-default collection found: ' + collection.name)
        finally:
            bpy.context.window.scene = oldscene
        return {'FINISHED'}

def menu_func(self, context):
    self.layout.operator(ExportBatchGoods.bl_idname, text="Batch Export Flan Goods")

def register():
    bpy.utils.register_class(ExportBatchGoods)
    bpy.types.TOPBAR_MT_file_export.append(menu_func)

def unregister():
    bpy.types.TOPBAR_MT_file_export.remove(menu_func)
    bpy.utils.unregister_class(ExportBatchGoods)

if __name__ == "__main__":
    register()
