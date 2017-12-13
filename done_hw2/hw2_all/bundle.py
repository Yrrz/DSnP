#! /usr/bin/env python3

import os.path
import fnmatch
import re
import argparse
import tarfile

parser = argparse.ArgumentParser(description='DSnP bundle tool.')
parser.add_argument('student_id', nargs='?', default='b0x901xxx',
                    help='your student_id.')
parser.add_argument('directory', help='homework directory.')

def check_remove(mrlist, name):
    for exclude in mrlist:
        if fnmatch.fnmatch(name, exclude):
            return True
    return False

def bundle(stu_id, hmdir):
    hmdir = os.path.normpath(hmdir)
    if not os.path.exists(hmdir):
        print("the directory {} doesn't exist.".format(hmdir))
        return 1

    list_file = os.path.join(hmdir, "MustExist.txt")
    if not os.path.isfile(list_file):
        print("MustExist.txt doesn't exist in {}.".format(hmdir))
        return 1

    file_list = []
    outprefix = '{}_{}'.format(stu_id, hmdir)
    with open(list_file, "r") as f:
        for line in f:
            filename = re.sub(r'[^/]*/(.*)', r'\1', line.strip())
            filename = os.path.join(hmdir, filename)
            file_list.append(filename)
        outprefix = stu_id + re.sub(r'([^/]*)/.*', r'\1', line.strip())

    mr_file = os.path.join(hmdir, "MustRemove.txt")
    mrlist = []
    if os.path.isfile(mr_file):
        with open(mr_file, "r") as f:
            for line in f:
                filename = re.sub(r'[^/]*/(.*)', r'\1', line.strip())
                filename = os.path.join(hmdir, filename)
                mrlist.append(filename)

    losts = [x for x in file_list if (not os.path.isfile(x)
                                      and not os.path.isdir(x))]
    if losts:
        for file in losts:
            print("{} doesn't exist.".format(file))
        return 1

    tarpath = os.path.join(hmdir, "{}.tgz".format(outprefix))
    tar = tarfile.open(tarpath, "w:gz")
    for filename in file_list:
        outname = re.sub(hmdir, outprefix, filename)
        tar.add(filename, outname, exclude=lambda name: check_remove(mrlist, name))

    tar.close()
    print('successfully create "{}"!'.format(tarpath))
    return 0

if __name__ == "__main__":
    args = parser.parse_args()
    ret = bundle(args.student_id, args.directory)
    exit(ret)
