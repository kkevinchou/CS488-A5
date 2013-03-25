-- A simple scene with some miscellaneous geometry.

off_white_reflective = gr.material({0.85, 0.8, 0.85}, {0.5, 0.4, 0.8}, 100, 0, 0.2)
off_white = gr.material({0.85, 0.8, 0.85}, {0.5, 0.4, 0.8}, 100)
green = gr.material({0.3, 0.6, 0.3}, {0.5, 0.7, 0.5}, 100)
blue = gr.material({0.4, 0.6, 1.0}, {0.5, 0.4, 0.8}, 100)
sphere_mat = gr.material({0.937, 0.294, 0.298}, {0.5, 0.7, 0.5}, 25, 0, 0.2)
cube_mat = gr.material({0.85, 0.85, 0.85}, {0.5, 0.7, 0.5}, 25, 0, 1)

scene_root = gr.node('root')

cornell = gr.node('cornell-box')
scene_root:add_child(cornell)

c1 = gr.cube('left')
c1:translate(-1.5, -0.5, 0)
c1:set_material(green)
cornell:add_child(c1)

c2 = gr.cube('right')
c2:translate(0.5, -0.5, 0)
c2:set_material(blue)
cornell:add_child(c2)

c3 = gr.cube('top')
c3:translate(-0.5, 0.5, 0)
c3:set_material(off_white)
cornell:add_child(c3)

c4 = gr.cube('bottom')
c4:translate(-0.5, -1.5, 0)
c4:set_material(off_white_reflective)
cornell:add_child(c4)

c5 = gr.cube('back')
c5:translate(-0.5, -0.5, -1)
c5:set_material(off_white)
cornell:add_child(c5)

test_sphere = gr.sphere('test_sphere')
test_sphere:translate(-0.2, -0.4, 0.6)
test_sphere:scale(0.1, 0.1, 0.1)
test_sphere:set_material(sphere_mat)
cornell:add_child(test_sphere)

test_box = gr.cube('testCube')
test_box:translate(0, -0.5, 0.4)
test_box:rotate('y', 45)
test_box:scale(0.15, 0.6, 0.45)
test_box:set_material(cube_mat)
cornell:add_child(test_box)

white_light = gr.light({-100.0, 150.0, 400.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
orange_light = gr.light({400.0, 100.0, 150.0}, {0.7, 0.0, 0.7}, {1, 0, 0})

box_light = gr.light({0, 0.2, 1}, {0.9, 0.9, 0.9}, {1, 0, 0}, {0.1, 0, 0}, {0, -0.1, 0})
-- box_light = gr.light({0, 0, 1}, {0.9, 0.9, 0.9}, {1, 0, 0})

-- gr.render(scene_root, 'scene.png', 500, 500,
--    {0, 0, -1.9}, {0, 0, -1}, {0, 1, 0}, 50,
--    {0.0, 0.0, 0.0}, {box_light})

gr.render(scene_root, 'scene.png', 500, 500,
      {0, 0, 2}, {0, 0, -1}, {0, 1, 0}, 50,
      {0.15, 0.15, 0.15}, {box_light})