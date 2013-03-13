-- A more macho version of simple_cows.py, in which cows aren't
-- spheres, they're cow-shaped polyhedral models.


-- We'll need an extra function that knows how to read Wavefront .OBJ
-- files.

require('readobj')

stone = gr.material({0.8, 0.7, 0.7}, {0.0, 0.0, 0.0}, 0)
grass = gr.material({1, 0.93, 0.65}, {0.0, 0.0, 0.0}, 0)
hide = gr.material({0.74, 0.5, 0.43}, {0.3, 0.3, 0.3}, 20)
wood = gr.material({0.66, 0.27, 0.05}, {0.0, 0.0, 0.0}, 0)

-- ##############################################
-- the scene
-- ##############################################

scene = gr.node('scene')
scene:rotate('X', 23)

-- the floor

plane = gr.mesh('plane', {
           { -1, 0, -1 },
           {  1, 0, -1 },
           {  1,  0, 1 },
           { -1, 0, 1  }
        }, {
           {3, 2, 1, 0}
        })
scene:add_child(plane)
plane:set_material(grass)
plane:scale(30, 30, 30)

--------------------------------------------------------
-- COW MESH
--------------------------------------------------------

cow_poly = gr.mesh('cow', readobj('cow.obj'))
factor = 2.0/(2.76+3.637)

cow_poly:set_material(hide)

cow_poly:translate(0.0, -1.0, 0.0)
cow_poly:scale(factor, factor, factor)
cow_poly:translate(0.0, 3.637, 0.0)

--------------------------------------------------------
-- FENCE MESH
--------------------------------------------------------

fence_poly = gr.nh_box('fence_poly', {0, 0, 0}, 1)
fence_poly:set_material(wood)

--------------------------------------------------------
-- RENDER COWS
--------------------------------------------------------

cow_number = 1

for _, pt in pairs({
              -- {{-8,1.3,0}, 225},
              -- {{-4,1.3,0}, 225},
              -- {{0,1.3,0}, 225},
              -- {{-8,1.3,4}, 225},
              -- {{-4,1.3,4}, 225},
              -- {{0,1.3,4}, 225},
              {{-8.7,1.3,10.2}, 65},
              {{-6,1.3,8}, 151},
              {{-2,1.3,8}, 225},
              }) do
   cow_instance = gr.node('cow' .. tostring(cow_number))
   scene:add_child(cow_instance)
   cow_instance:add_child(cow_poly)
   cow_instance:translate(unpack(pt[1]))
   cow_instance:rotate('Y', pt[2])
   cow_instance:scale(1.4, 1.4, 1.4)

   cow_number = cow_number + 1
end

--------------------------------------------------------
-- RENDER FENCES
--------------------------------------------------------

-- FRONT

fence_y = 0.3
fence_z = 0.3

front_fence_number = 1

for _, pt in pairs({
              {{-9, 1, 15}, 30},
              {{-9, 2, 15}, 30},
              }) do
   fence_instance = gr.node('fence' .. tostring(front_fence_number))
   scene:add_child(fence_instance)
   fence_instance:add_child(fence_poly)
   fence_instance:translate(unpack(pt[1]))
   fence_instance:rotate('Y', pt[2])
   fence_instance:scale(14, fence_y, fence_z)

   front_fence_number = front_fence_number + 1
end

-- BACK

back_fence_number = 100

for _, pt in pairs({
              {{3, 1, 8}, 150},
              {{3, 2, 8}, 150},
              }) do
   fence_instance = gr.node('fence' .. tostring(back_fence_number))
   scene:add_child(fence_instance)
   fence_instance:add_child(fence_poly)
   fence_instance:translate(unpack(pt[1]))
   fence_instance:rotate('Y', pt[2])
   fence_instance:scale(23, fence_y, fence_z)

   back_fence_number = back_fence_number + 1
end

-- FRONT POLES
fence_pole_front_num = 0
fence_pole_front_x = 1
fence_pole_front_z = 8

for _, pt in pairs({
              {{fence_pole_front_x, 0, fence_pole_front_z}, 30}, -- front fence
              {{fence_pole_front_x, 0, fence_pole_front_z}, 30}, -- front fence
              {{fence_pole_front_x, 0, fence_pole_front_z}, 30}, -- front fence
              {{fence_pole_front_x, 0, fence_pole_front_z}, 30}, -- front fence
              {{fence_pole_front_x, 0, fence_pole_front_z}, 30}, -- front fence
              {{fence_pole_front_x, 0, fence_pole_front_z}, 30}, -- front fence
              {{fence_pole_front_x, 0, fence_pole_front_z}, 30}, -- front fence
              {{fence_pole_front_x, 0, fence_pole_front_z}, 30}, -- front fence
              }) do
   fence_instance = gr.node('fence' .. tostring(fence_number))
   scene:add_child(fence_instance)
   fence_instance:add_child(fence_poly)
   fence_instance:translate(unpack(pt[1]))
   fence_instance:rotate('Y', pt[2])
   fence_instance:translate(-fence_pole_front_num * 2, 0, 0)
   fence_instance:scale(fence_z, fence_y * 7, fence_z)

   fence_pole_front_num = fence_pole_front_num + 1
end

-- BACK POLES

fence_pole_back_num = 0
fence_pole_back_x = 1
fence_pole_back_z = 8
fence_pole_back_rot = -30

for _, pt in pairs({
              {{fence_pole_back_x, 0, fence_pole_back_z}, fence_pole_back_rot},
              {{fence_pole_back_x, 0, fence_pole_back_z}, fence_pole_back_rot},
              {{fence_pole_back_x, 0, fence_pole_back_z}, fence_pole_back_rot},
              {{fence_pole_back_x, 0, fence_pole_back_z}, fence_pole_back_rot},
              {{fence_pole_back_x, 0, fence_pole_back_z}, fence_pole_back_rot},
              {{fence_pole_back_x, 0, fence_pole_back_z}, fence_pole_back_rot},
              {{fence_pole_back_x, 0, fence_pole_back_z}, fence_pole_back_rot},
              {{fence_pole_back_x, 0, fence_pole_back_z}, fence_pole_back_rot},
              {{fence_pole_back_x, 0, fence_pole_back_z}, fence_pole_back_rot},
              {{fence_pole_back_x, 0, fence_pole_back_z}, fence_pole_back_rot},
              {{fence_pole_back_x, 0, fence_pole_back_z}, fence_pole_back_rot},
              }) do
   fence_instance = gr.node('fence' .. tostring(fence_number))
   scene:add_child(fence_instance)
   fence_instance:add_child(fence_poly)
   fence_instance:translate(unpack(pt[1]))
   fence_instance:rotate('Y', pt[2])
   fence_instance:translate(-fence_pole_back_num * 2, 0, -1)
   fence_instance:scale(fence_z, fence_y * 9, fence_z)

   fence_pole_back_num = fence_pole_back_num + 1
end

--------------------------------------------------------
-- BARN MESH
--------------------------------------------------------
barn = gr.node('barn')

-- BARN BASE

barn_poly = gr.cube('barn_poly')
barn_poly:set_material(wood)
barn:add_child(barn_poly)

--ROOF MESH

roof = gr.mesh('roof', {
           { 0, 0, 0 },
           { 1, 0, 0 },
           { 0.5,  1, 0 },
        }, {
           {0, 1, 2}
        })
barn:add_child(roof)
roof:set_material(hide)
roof:translate(0, 1, 0)

-- BARN

scene:add_child(barn)
barn:translate(-4, 0, -7)
barn:rotate('Y', 50)
barn:scale(13, 8, 16)

gr.render(scene,
      'macho_cows.png', 500, 500,
      {0, 2, 30}, {0, 0, -1}, {0, 1, 0}, 50,
      {0.4, 0.4, 0.4}, {gr.light({200, 302, 430}, {0.8, 0.8, 0.8}, {1, 0, 0})})
