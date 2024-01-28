import lit.formats
config.name = "Oberon"
config.test_format = lit.formats.ShTest(True)
config.suffixes = ['.mod']
config.excludes = ['tmp', 'lib', 'include']
config.test_source_root = os.path.dirname(__file__)
config.test_exec_root = os.path.join(os.path.dirname(__file__), "tmp")
inc = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "oberon", "include"))
lib = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "oberon", "lib"))
config.substitutions.append((r"%inc", inc))
config.substitutions.append((r"%lib", lib))
config.substitutions.append((r"%oberon", r"oberon-lang"))