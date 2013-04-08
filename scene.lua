-- A simple scene with some miscellaneous geometry.

crate_mat = gr.texture_material("../data/crate2.png", {0.5, 0.4, 0.8}, 100)
metal_ref_mat = gr.texture_material("../data/metal1.png", {0.5, 0.4, 0.8}, 100, 0.06, 0.35)
metal_mat = gr.texture_material("../data/metal1.png", {0.5, 0.4, 0.8}, 100, 0.06, 0.35)
life_saver_mat = gr.texture_material("../data/stripe.png", {0.5, 0.4, 0.8}, 100)

off_white_reflective = gr.material({0.85, 0.8, 0.85}, {0.5, 0.4, 0.8}, 100, 0.06, 0.35)
off_white = gr.material({0.85, 0.8, 0.85}, {0.5, 0.4, 0.8}, 100)
green = gr.material({0.3, 0.6, 0.3}, {0.5, 0.7, 0.5}, 100)
blue = gr.material({0.4, 0.6, 1.0}, {0.5, 0.4, 0.8}, 100)

scene_root = gr.node('root')

plane = gr.node('plane')
scene_root:add_child(plane)

c1 = gr.cube('left')
c1:translate(-3.5, -0.5, 0)
c1:scale(1, 1, 4)
c1:set_material(off_white)
plane:add_child(c1)

c2 = gr.cube('right')
c2:translate(2.5, -0.5, 0)
c2:scale(1, 1, 4)
c2:set_material(off_white)
plane:add_child(c2)

c3 = gr.cube('top')
c3:translate(-2.5, 0.5, 0)
c3:scale(5, 1, 4)
c3:set_material(off_white_reflective)
plane:add_child(c3)

c4 = gr.cube('bottom')
c4:translate(-2.5, -1.5, 0)
c4:scale(5, 1, 4)
c4:set_material(off_white)
plane:add_child(c4)

c5 = gr.cube('back1')
c5:translate(-2.5, -0.5, 0)
c5:scale(2, 1, 0.1)
c5:set_material(off_white)
plane:add_child(c5)

crate1 = gr.cube('crate1')
crate1:translate(-1.2, -0.5, 0.1)
crate1:scale(0.4, 0.3, 0.3)
crate1:set_material(blue)
plane:add_child(crate1)

life_saver = gr.torus('life_saver')
life_saver:translate(-1, 0.2, 0.1)
life_saver:scale(0.1, 0.1, 0.1)
life_saver:set_material(green)
plane:add_child(life_saver)

-----------------------------------------------------------
-- WINDOW WALL
-----------------------------------------------------------

window_wall = gr.node('window_wall')
window_wall:translate(0.5, -0.5, 0)
plane:add_child(window_wall)

window_bottom = gr.cube('window_bottom')
window_bottom:translate(0, 0, 0)
window_bottom:scale(2, 0.3, 0.1)
window_bottom:set_material(off_white)
window_wall:add_child(window_bottom)

window_top = gr.cube('window_top')
window_top:translate(0, 0.7, 0)
window_top:scale(2, 0.3, 0.1)
window_top:set_material(off_white)
window_wall:add_child(window_top)

window_left = gr.cube('window_left')
window_left:translate(0, 0.3, 0)
window_left:scale(0.4, 0.4, 0.1)
window_left:set_material(off_white)
window_wall:add_child(window_left)

window_right = gr.cube('window_right')
window_right:translate(1.6, 0.3, 0)
window_right:scale(0.4, 0.4, 0.1)
window_right:set_material(off_white)
window_wall:add_child(window_right)

-----------------------------------------------------------
-- BENCH
-----------------------------------------------------------

-- leg_length = 0.1
-- leg_dim = 0.05
-- bench_length = 0.8
-- bench_dim = 0.3

-- bench = gr.node('bench')
-- bench:translate(0.8, -0.4, 0.4)
-- plane:add_child(bench)

-- bench_body = gr.cube('bench_body')
-- bench_body:scale(bench_length, 0.05, bench_dim)
-- bench_body:set_material(blue)
-- bench:add_child(bench_body)

-- leg_front_left = gr.cube('leg_front_left')
-- leg_front_left:translate(0, -1 * leg_length, 0)
-- leg_front_left:scale(leg_dim, leg_length, leg_dim)
-- leg_front_left:set_material(blue)
-- bench:add_child(leg_front_left)

-- leg_front_right = gr.cube('leg_front_right')
-- leg_front_right:translate(bench_length - leg_dim, -1 * leg_length, 0)
-- leg_front_right:scale(leg_dim, leg_length, leg_dim)
-- leg_front_right:set_material(blue)
-- bench:add_child(leg_front_right)

-- leg_back_left = gr.cube('leg_back_left')
-- leg_back_left:translate(0, -1 * leg_length, bench_dim - leg_dim)
-- leg_back_left:scale(leg_dim, leg_length, leg_dim)
-- leg_back_left:set_material(blue)
-- bench:add_child(leg_back_left)

-- leg_back_right = gr.cube('leg_back_right')
-- leg_back_right:translate(bench_length - leg_dim, -1 * leg_length, bench_dim - leg_dim)
-- leg_back_right:scale(leg_dim, leg_length, leg_dim)
-- leg_back_right:set_material(blue)
-- bench:add_child(leg_back_right)

-----------------------------------------------------------
-- LIGHTS
-----------------------------------------------------------

white_light = gr.light({-100.0, 150.0, 400.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
orange_light = gr.light({400.0, 100.0, 150.0}, {0.7, 0.0, 0.7}, {1, 0, 0})

box_light = gr.light({0, 0, 1}, {0.9, 0.9, 0.9}, {1, 0, 0})
-- window_light = gr.light({0.9, 0.2, 0.5}, {1.5, 1.5, 1.5}, {1, 0, 0}, {1.2, 0, 0}, {0, -0.4, 0})
-- box_light = gr.light({0, 0, 1}, {0.9, 0.9, 0.9}, {1, 0, 0}, {0.1, 0, 0}, {0, -0.1, 0})

-- gr.render(scene_root, 'scene.png', 500, 500,
--    {0, 0, -1.9}, {0, 0, -1}, {0, 1, 0}, 50,
--    {0.0, 0.0, 0.0}, {box_light})

t1 = gr.tween('translate', 0, 1, {0, 0, 2.1})
t2 = gr.tween('rotate', 0, 1, {-0.4, 0, -1})

gr.render(scene_root, 'scene.png', 500, 500,
      {0, 0, 2.3}, {0.4, 0, -1}, {0, 1, 0}, 50,
      {0.25, 0.25, 0.25}, {box_light}, {t2})