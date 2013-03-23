-- A simple scene with some miscellaneous geometry.

mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25)
mat2 = gr.material({0.5, 0.5, 0.5}, {0.5, 0.7, 0.5}, 25)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25)
mat4 = gr.material({0.4, 0.6, 1.0}, {0.5, 0.4, 0.8}, 25)

scene_root = gr.node('root')
scene_root:translate(0, -0.5, -4)

cornell = gr.node('cornell-box')
scene_root:add_child(cornell)

c1 = gr.cube('left')
c1:translate(-1.5, 0, 0)
c1:set_material(mat4)
cornell:add_child(c1)

c2 = gr.cube('right')
c2:translate(0.5, 0, 0)
c2:set_material(mat4)
cornell:add_child(c2)

c3 = gr.cube('top')
c3:translate(-0.5, 1, 0)
c3:set_material(mat4)
cornell:add_child(c3)

c4 = gr.cube('bottom')
c4:translate(-0.5, -1, 0)
c4:set_material(mat4)
cornell:add_child(c4)

c5 = gr.cube('back')
c5:translate(-0.5, 0, -1)
c5:set_material(mat4)
cornell:add_child(c5)

test_sphere_pos = gr.node('test_sphere_pos')
test_sphere_pos:translate(0, 0.5, 4)
test_sphere_pos:translate(0, 0, 0)
cornell:add_child(test_sphere_pos)

-- test_sphere = gr.sphere('testSphere')
-- test_sphere:scale(0.1, 0.1, 0.1)
-- test_sphere:set_material(mat4)
-- test_sphere_pos:add_child(test_sphere)

white_light = gr.light({-100.0, 150.0, 400.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
orange_light = gr.light({400.0, 100.0, 150.0}, {0.7, 0.0, 0.7}, {1, 0, 0})

box_light = gr.light({0, 0, -3.25}, {0.9, 0.9, 0.9}, {1, 0, 0})

-- gr.render(scene_root, 'scene.png', 500, 500,
--    {0, 0, -1.9}, {0, 0, -1}, {0, 1, 0}, 50,
--    {0.0, 0.0, 0.0}, {box_light})

gr.render(scene_root, 'scene.png', 500, 500,
      {0, 0, -2}, {0, 0, -1}, {0, 1, 0}, 50,
      {0.0, 0.0, 0.0}, {box_light})