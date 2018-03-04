
#include "WaterSim.class.hpp"

int	main(int ac, char **av)
{
	if (ac != 2)
		return 1;

	Window window(X_DIM, Y_DIM, "watersim");
	Camera camera;
	WaterSim simulator(av[1]);

	window.EventListen();
	window.ForwardAndBackKeys('W', 'S');
	window.LeftAndRightKeys('A', 'D');
	window.UpAndDownKeys('Z', 'X');

	camera.TrackEvents(&window);	
	camera.Move(glm::vec3(0, 0, 300));
	camera.Rotate(glm::vec3(0, 1, 0), 180.0f);
	

	glClearColor(0.7, 0.7, 0.8, 1);
	float h = 0;
	while (window.IsOpen())
	{
		window.Clear();
		camera.Update();
		if (camera.JustMoved())			
			simulator.NewPerspective(camera.Perspective());
		simulator.DrawLand();
		simulator.DrawWater();
		if (window.KeyOn('1'))
			simulator.WaveIteration();
		if (window.KeyOn('2'))
			simulator.RainIteration();
		if (window.KeyOn('3'))
			simulator.DrainIteration();
		if (window.KeyOn('4'))
			simulator.SinkIteration();
		if (window.KeyOn('5'))
			simulator.FloodIteration();
		if (window.KeyOn('6'))
		{
			simulator.RisingLevelIteration(h);
			h += 0.05;
		}
		else
			h = 0;
		simulator.WaterIteration();
		window.Update();		
	}
}
