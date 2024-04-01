variants = {"RSDP", "RSDPG"}

categories = {128: 1,
              192: 3,
              256: 5}

targets = {"small": "SIG_SIZE",
           "balanced": "BALANCED",
           "fast": "SPEED"}

implementations = {"clean"}

def generate_namespace(variant, category, target, implementation):
  return f"PQCLEAN_CROSS{variant}{category}{target}_{implementation}_".upper()

def generate_dir_name(variant, category, target, implementation):
  return f"/cross-{variant}-{category}-{target}/{implementation}".lower()