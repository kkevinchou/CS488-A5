-- A more macho version of simple_cows.py, in which cows aren't
-- spheres, they're cow-shaped polyhedral models.


-- We'll need an extra function that knows how to read Wavefront .OBJ
-- files.

require('readobj')

stone = gr.material({0.8, 0.7, 0.7}, {0.0, 0.0, 0.0}, 0)
grass = gr.material({0.1, 0.7, 0.1}, {0.0, 0.0, 0.0}, 0)
hide = gr.material({0.84, 0.6, 0.53}, {0.3, 0.3, 0.3}, 20)

cow_poly = gr.mesh('dragon', readobj('dragon.obj'))
factor = 2.0/(2.76+3.637)

cow_poly:set_material(hide)

cow_poly:translate(0.0, -1.0, 0.0)
cow_poly:scale(factor, factor, factor)
cow_poly:translate(0.0, 3.637, 0.0)

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

-- Use the instanced cow model to place some actual cows in the scene.
-- For convenience, do this in a loop.

cow_number = 1

for _, pt in pairs({
              {{1,1.3,14}, 20}}) do
   cow_instance = gr.node('cow' .. tostring(cow_number))
   scene:add_child(cow_instance)
   cow_instance:add_child(cow_poly)
   cow_instance:translate(unpack(pt[1]))
   cow_instance:rotate('Y', pt[2])
   cow_instance:scale(1.4, 1.4, 1.4)

   cow_number = cow_number + 1
end

gr.render(scene,
      'sample.png', 256, 256,
      {0, 2, 30}, {0, 0, -1}, {0, 1, 0}, 50,
      {0.4, 0.4, 0.4}, {gr.light({200, 202, 430}, {0.8, 0.8, 0.8}, {1, 0, 0})})
