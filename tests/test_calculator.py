# @relation(ARCH-1, scope=file, role=Test)

from src.calculator import add


def test_add():
    assert add(2, 3) == 5


def test_add_rejects_non_numeric():
    try:
        add("2", 3)
    except TypeError:
        pass
    else:
        raise AssertionError("expected TypeError")
