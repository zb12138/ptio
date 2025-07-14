from setuptools import setup, Extension
ext_modules = [
    Extension(
        'ptio_backend',
        ['backend.cpp',
        'ply.cpp',
        'table.cpp'
        ],
        language='c++',
        extra_compile_args=[
            '-std=c++11',
            '-Wno-sign-compare',
            '-Wno-parentheses',     
            '-Wno-unused-but-set-variable',
            '-Wno-ignored-qualifiers',
        ]
    ),
]

setup(
    name='ptio',
    version='25.7.2',
    ext_modules=ext_modules,
    py_modules=['ptio'],
    zip_safe=False,
    author='chunyangfu',
    author_email='chunyang.fu@my.cityu.edu.hk',
    description='Fast point cloud reading and writing library',
    url='https://github.com/zb12138/ptio.git',
)