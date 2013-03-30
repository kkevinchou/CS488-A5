-- A simple scene with some miscellaneous geometry.

test_material = gr.texture_material("../data/tex1.png", {0.5, 0.4, 0.8}, 100, 0.06, 0.55)

off_white_reflective = gr.material({0.85, 0.8, 0.85}, {0.5, 0.4, 0.8}, 100, 0.06, 0.55)
off_white = gr.material({0.85, 0.8, 0.85}, {0.5, 0.4, 0.8}, 100)
green = gr.material({0.3, 0.6, 0.3}, {0.5, 0.7, 0.5}, 100)
blue = gr.material({0.4, 0.6, 1.0}, {0.5, 0.4, 0.8}, 100)
sphere_mat = gr.material({0.937, 0.294, 0.298}, {0.5, 0.7, 0.5}, 25, 0, 0.2)
cube_mat = gr.material({0.85, 0.85, 0.85}, {0.5, 0.7, 0.5}, 25, 0, 1)
cylinder_mat = gr.material({0.85, 0.85, 0.85}, {0.5, 0.7, 0.5}, 25, 0, 1)

scene_root = gr.node('root')

cornell = gr.node('cornell-box')
scene_root:add_child(cornell)

sphere = gr.sphere('sphere')
sphere:translate(0, 0, -5)
-- sphere:rotate('y', 90)
-- sphere:rotate('x', 45)
sphere:set_material(test_material)
cornell:add_child(sphere)

white_light = gr.light({-100.0, 150.0, 400.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
orange_light = gr.light({400.0, 100.0, 150.0}, {0.7, 0.0, 0.7}, {1, 0, 0})

box_light = gr.light({0, 0, 1}, {0.9, 0.9, 0.9}, {1, 0, 0})
-- box_light = gr.light({0, 0, 1}, {0.9, 0.9, 0.9}, {1, 0, 0}, {0.1, 0, 0}, {0, -0.1, 0})

-- gr.render(scene_root, 'scene.png', 500, 500,
--    {0, 0, -1.9}, {0, 0, -1}, {0, 1, 0}, 50,
--    {0.0, 0.0, 0.0}, {box_light})

gr.render(scene_root, 'scene.png', 500, 500,
      {0, 0, 2}, {0, 0, -1}, {0, 1, 0}, 50,
      {0.15, 0.15, 0.0}, {box_light})