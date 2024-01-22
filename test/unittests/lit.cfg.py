import lit.formats
config.name = "Oberon"
config.test_format = lit.formats.ShTest(True)
config.suffixes = ['.mod']
config.excludes = ['tmp']
config.test_source_root = os.path.dirname(__file__)
config.test_exec_root = os.path.join(os.path.dirname(__file__), "tmp")
config.substitutions.append((r"%oberon", r"oberon-lang"))