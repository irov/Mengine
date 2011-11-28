import MyModule

b = MyModule.Bar()

b.foo()
b.min(1,2)

MyModule.test(b)

b.data = 1
print b.data

b.data = 3
print b.data

b.helper_for_script_function(1,2)

v = MyModule.Vec2(2,3)

print v.x, v.y

v.x = 1
v.y = 1

print v.x, v.y
