# Based on: https://github.com/seqan/seqan3-doc/blob/user/source/make_source.py
# Copied Jan 3, 2018
# Heavily modified by B. LaRoque and N. Oblath

# Original version:
#  written by Jongkyu Kim (j.kim@fu-berlin.de)

import os
import sys

class site_builder:
    def __init__(self):
        cat_name = "API Reference"
        index_temp = "./_index.rst"
        index_final = "index.rst"
        api_sub_dir = "API_Ref"
        toc_end = ".. end of toc\n"

    def generateIndex(self, outDir):
        print('[make_source] Generating index')
    
        # generate index.rst
        inFile = open(self.index_temp, "r")
        outFile = open(os.path.join(outDir, self.index_final), "w")
        for line in inFile :
            outFile.write(line)
        inFile.close()
        outFile.close()
    
        # generate API_Ref/index.rst
        api_path = os.path.join(outDir, self.api_sub_dir)
        self.mkAPISubdir(api_path, self.api_sub_dir)
    
    def addTOCEntry(self, toc_path, new_item, caption=None):
        #print("## adding {}".format(new_item.replace("/", " > ")))
        #if not caption:
        #    caption = None
        caption = None
        ## todo captions?
        the_lines = open(toc_path).readlines()
        end_of_tree = len(the_lines)
        try:
            end_of_tree = the_lines.index(self.toc_end)
        except ValueError:
            print("[make_source] WARNING!! someone removed the end of ToC comment marker!\n     the jerk ... assuming EOF")
        the_lines.insert(end_of_tree, "   {}\n".format(new_item))
        if caption is not None:
            the_lines.insert(end_of_tree, "   :caption: {}\n".format(caption))
        f = open(toc_path, "w")
        f.writelines(the_lines)
        f.close()
    
    def mkAPISubdir(dirname, title, caption=True):
        if caption is True:
            caption = title
    
        # make the directory
        if not os.path.isdir(dirname):
            os.mkdir(dirname)
    
        # make the index file in the directory
        api_toc_file = open(os.path.join(dirname, "index.rst"), 'w')
        api_toc_file.write("{}\n{}\n\n".format(title, '='*len(title)))
        api_toc_file.write(".. toctree::\n   :maxdepth: 2\n\n")
        api_toc_file.write(self.toc_end)
        api_toc_file.close()
    
        # add the subdir's index to the parent's ToC
        self.addTOCEntry(os.path.join(dirname, '../index.rst'), '{}/index'.format(os.path.relpath(dirname).split('/')[-1]), caption=caption)
    
    def generateFileRST(self, outDir, moduleName, fileName) :
        print('[make_source] Generating File RST: {} {} {}'.format(outDir, moduleName, fileName))
    
        # title
        out_file_path = os.path.join(outDir,fileName.split('../',1)[-1]) + ".rst"
        outFile = open(out_file_path, "w")
        title = '/'.join([moduleName, fileName])
        title = title.lstrip('/.')
        print('title is: {}'.format(title))
        outFile.write(title + "\n")
        outFile.write("=" * len(title) + "\n\n")
    
        # doxgenfile
        outFile.write(".. doxygenfile:: {}\n".format(fileName.lstrip('./')))
        outFile.write("   :project: myproject\n\n")
    
        outFile.close()
    
        # add to ToC
        self.addTOCEntry(os.path.join(os.path.dirname(out_file_path), 'index.rst'), os.path.basename(out_file_path).rsplit('.rst')[0], caption=False)
    
    def generateRSTs(self, in_names, outDir, excl_names):
        print('[make_source] Generating RSTs: {} {}'.format(in_names, outDir))
        listModules = []
        listFiles = []
        for fileName in in_names:
            if os.path.normpath(fileName) in (os.path.normpath(a_path) for a_path in excl_names):
                continue
            if os.path.isdir(fileName):
                listModules.append(fileName)
            elif fileName.endswith(('.hh', '.cc')):
                listFiles.append(fileName)
    
        listModules = sorted(listModules)
        listFiles = sorted(listFiles)
        print("[make_source] modules: {}".format(listModules))
        print("[make_source] files: {}".format(listFiles))
    
        api_dir = os.path.join(outDir, self.api_sub_dir)
        moduleName = ''
        for a_file in listFiles :
            #print('> fn: {}'.format(a_file))
            print('[make_source] (api_dir, module, file) -> ({}, {}, {})'.format(api_dir, moduleName, a_file))
            self.generateFileRST(api_dir, moduleName, a_file)
    
        for a_module in listModules:
            #print("module: {} {}".format(a_module, '-'*30))
            a_name = a_module.split('../', 1)[-1]
            self.mkAPISubdir(os.path.join(api_dir, a_name), a_name)
            self.generateRSTs([os.path.join(a_module, f) for f in os.listdir(a_module)], outDir, excl_names)
    
    
    
    ###################
    
    # usage: make_source.build(path_to_where_doc_rst_files_live, [paths to each directory containing source with doxygen], [paths to directories to exclude])
    
    # you should run this file early from within your conf.py
    def build(self, outDir, inDirs, exclDirs):
        self.generateIndex(outDir)
        self.generateRSTs(inDirs, outDir, exclDirs)
