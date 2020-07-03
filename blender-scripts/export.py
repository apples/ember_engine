import os, struct, math
import mathutils
import bpy
import bpy_extras.io_utils
import argparse
import sys
import shutil

# Filter args to those after --
argv = sys.argv
if "--" not in argv:
    argv = []
else:
    argv = argv[argv.index("--") + 1:]

# Parse args
parser = argparse.ArgumentParser()
parser.add_argument('objectname')
parser.add_argument('modelout')
parser.add_argument('textureout')
args = parser.parse_args(argv)

def export_obj(obj, objpath):
    bpy.ops.scene.new()
    bpy.context.scene.collection.objects.link(obj)
    obj.select_set(True)
    bpy.ops.export_scene.obj(
        filepath=objpath,
        use_selection=True,
        use_edges=False,
        use_smooth_groups=False,
        use_materials=False,
        use_triangles=True)

def export_iqm(obj, iqmpath):
    bpy.ops.scene.new()

    armature = None

    if obj.type == 'MESH' and obj.parent is not None and obj.parent.type == 'ARMATURE':
        armature = obj.parent
    elif obj.type == 'ARMATURE':
        armature = obj
    
    animspec = ''
    
    if armature is not None:
        bpy.context.scene.collection.objects.link(armature)
        armature.select_set(True)
        for c in armature.children:
            bpy.context.scene.collection.objects.link(c)
            c.select_set(True)
        animactions = []
        if armature.animation_data is not None:
            for t in armature.animation_data.nla_tracks:
                for s in t.strips:
                    animactions.append(s.name)
        animspec = ','.join(animactions)
    else:
        bpy.context.scene.collection.objects.link(obj)
        obj.select_set(True)

    bpy.ops.export.iqm(
        filepath=iqmpath,
        useskel=armature is not None,
        animspec=animspec,
        matfmt='i')

def export_png(img, imgpath):
    if img.filepath is not None and img.filepath != '':
        sourcepath = bpy.path.abspath(img.filepath, library=img.library)
        print(f'Copying image "{sourcepath}" => "{imgpath}"')
        shutil.copyfile(sourcepath, imgpath)
    else:
        print(f'Unpacking image {img.name} => "{imgpath}"')
        img.filepath = imgpath
        img.save()

# Try export object
if args.objectname in bpy.data.objects:
    fileext = os.path.splitext(args.modelout)
    (export_obj if fileext == 'obj' else export_iqm)(bpy.data.objects[args.objectname], args.modelout)
else:
    raise Exception('Object not found!')

# Try export image
if args.objectname in bpy.data.images:
    export_png(bpy.data.images[args.objectname], args.textureout)
