import subprocess
import argparse
import re
import os
import sys
from pathlib import Path
import shutil
from collections.abc import Callable

PIPE = subprocess.PIPE

class SUCCESS(Exception): pass
class FAILED(Exception): pass

RED = "\033[31m" if sys.stdout.isatty() else ""
GREEN = "\033[32m" if sys.stdout.isatty() else ""
END = "\033[0m" if sys.stdout.isatty() else ""

_tests = []
def run_tests():
	for f in _tests:
		f()
	if _tests:
		print("------------------")
	else:
		print("NO TESTS")

def prep_targets(targets: list[str | Path]) -> list[Path]:
	tmp_targets = []
	for i in targets:
		bin_, tmp = get_file(i)
		tmp_targets.append(tmp)
		assert(file_exists(bin_)), f"{bin_} not found"
		if file_exists(tmp):
			tmp.unlink()
		shutil.copy(bin_, tmp)
	return tmp_targets

def test(executable: str='', targets: list[str | Path]=[], *, _cache=[0]):
	def decorator(f: Callable[[Path, list[Path]], bool]):
		n = _cache[0]
		_cache[0] += 1
		def wrapper() -> None:
			print(f"----- TEST {n} ----- {f.__name__}")
			print(f"{executable} on {', '.join(map(str, targets))}")
			try:
				assert file_exists(executable), f"executable {executable} not found"
				result = f(Path(executable).resolve(), prep_targets(targets))
				if result is None or result:
					raise SUCCESS()
				raise FAILED()
			except SUCCESS as e:
				print(GREEN, "OK", END, sep='')
			except (FAILED, AssertionError) as e:
				print(RED, "KO", END, sep='')
				if e.args:
					print(' '.join(map(str, e.args)))
			for i in Path('./tmp/').glob('*'):
				i.unlink()
		_tests.append(wrapper)
		return wrapper
	return decorator

def run(cmd: list[str]) -> tuple[bytes, bytes]:
	return subprocess.Popen(cmd, stdout=PIPE, stderr=PIPE).communicate()

CODED_BY = re.compile(r"Famine \(c\)oded by dxrxbxk.*\n")
def is_infected(path: Path | str):
	out, err = run(["strings", str(path)])
	if err:
		raise FAILED("cmd `strings` failed", err.decode())
	return CODED_BY.search(out.decode()) is not None

def file_exists(path: Path | str):
	return Path(path).exists()

def get_file(path: str | Path, _cache: dict[str, tuple[Path, Path]] = { k.name: (k, k.parent.with_name("tmp") / k.name) for k in Path('./bin/').glob("*") if os.access(k, os.X_OK)}) -> tuple[Path, Path]:
	return _cache[Path(path).name]

def list_chmod(targets: list[Path], perms: list[int]):
	for target, perm in zip(targets, perms):
		target.chmod(perm)

def test_basic(executable: Path, targets: list[Path]):
	before = {}
	for target in targets:
		before[target] = run([str(target)])
	out, err = run([str(executable)])
	if err:
		raise FAILED(out.decode())
	for target in targets:
		if not is_infected(target):
			raise FAILED(target, "TARGET NOT INFECTED")
		out, err = run([str(target)])
		if (out, err) != before[target]:
			raise FAILED(target, "OUTPUT CHANGED")
	return True

def test_reinfect(executable: Path, targets: list[Path]):
	perms: dict[Path, int] = {}

	for target in targets:
		perms[target] = target.stat().st_mode & 0o777
	test_basic(executable=executable, targets=targets)

	for target in targets:
		list_chmod(targets, [0o000] * len(targets))

		target.chmod(perms[target])
		test_basic(executable=target, targets=[target])
	list_chmod([*perms.keys()], [*perms.values()])
	return True

@test("./Famine", ["ls"])
def test_ls(executable: Path, targets: list[Path]):
	return test_basic(executable=executable, targets=targets)

@test("./Famine", ["ls", "hello_fini", "hello_nopie", "hello_nofini", "hello_printf"])
def test_all(executable: Path, targets: list[Path]):
	return test_basic(executable=executable, targets=targets)

@test("./Famine", ["ls"])
def test_reinfect_ls(executable: Path, targets: list[Path]):
	return test_reinfect(executable=executable, targets=targets)

@test("./Famine", ["ls", "hello_fini", "hello_nopie", "hello_nofini", "hello_printf"])
def test_reinfect_all(executable: Path, targets: list[Path]):
	return test_reinfect(executable=executable, targets=targets)


if __name__ == '__main__':
	run_tests()
