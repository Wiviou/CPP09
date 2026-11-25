#!/usr/bin/env python3
"""Optional evaluation tests. Run make, then python3 tests.py."""
import random
import re
import subprocess


def valid(numbers):
    result = subprocess.run(
        ['./PmergeMe'] + [str(n) for n in numbers],
        capture_output=True, text=True, check=True,
    )
    lines = result.stdout.splitlines()
    assert not result.stderr, result.stderr
    assert len(lines) == 4, lines
    assert lines[0].startswith('Before:'), lines
    assert lines[1].startswith('After:'), lines
    assert list(map(int, lines[0].split()[1:])) == numbers
    assert list(map(int, lines[1].split()[1:])) == sorted(numbers)
    for line, name in zip(lines[2:], ('vector', 'deque')):
        pattern = (r'Time to process a range of ' + str(len(numbers))
                   + r' elements with std::' + name + r' : [0-9.]+ us')
        assert re.fullmatch(pattern, line), line


def invalid(args):
    result = subprocess.run(['./PmergeMe'] + args,
                            capture_output=True, text=True)
    assert result.returncode > 0, (args, result)
    assert result.stderr.startswith('Error:'), (args, result)
    assert not result.stdout, (args, result)


rng = random.Random(42)
cases = [
    [3, 5, 9, 7, 4], [1], [2, 1], [1, 2], [1, 1],
    [4, 4, 1, 1, 3, 3, 2], [2147483647, 1, 2147483647],
    list(range(1, 3001)), list(range(3000, 0, -1)),
    [7] * 3000, rng.sample(range(1, 100001), 3000),
    [rng.randint(1, 1000) for _ in range(3000)],
    [rng.randint(1, 1000) for _ in range(3001)],
]
for n in range(1, 101):
    cases.append([rng.randint(1, 50) for _ in range(n)])
for case in cases:
    valid(case)

errors = [[], ['-1', '2'], ['0'], ['1', '0'], ['abc'], ['1.5'],
          ['2147483648'], ['99999999999999999999999'], [''],
          ['+'], ['--1'], ['3a'], ['1 2'], ['3', 'abc', '4']]
for case in errors:
    invalid(case)

print(f'OK: {len(cases)} valid cases and {len(errors)} error cases.')
