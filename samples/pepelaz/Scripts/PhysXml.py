#coding=UTF-8
pepelaz = {}

box = '''
<Entity>
	
	<PhysicBody>
		<Shape>
			<Point Value = "-25; -25"/>
			<Point Value = "25; -25"/>
			<Point Value = "25; 25"/>
			<Point Value = "-25; 25"/>
		</Shape>
		<Density Value = "0.001"/>
		<Friction Value = "0.8"/>
		<Restitution Value = "0.5"/>
	</PhysicBody>
	
	<Node Name = "Passive" Type = "Sprite">
		<ImageMap Name = "Box"/>
		<CenterAlign Value = "1"/>
	</Node>
	
</Entity>
'''

pepelaz["Chain"] = '''
<Entity>
	
	<PhysicBody>
		<Shape>
			<Point Value = "-5; -6"/>
			<Point Value = "5; -6"/>
			<Point Value = "5; 6"/>
			<Point Value = "-5; 6"/>
		</Shape>
		<Density Value = "0.003"/>
		<Friction Value = "0.8"/>
		<Restitution Value = "0.5"/>
		<GroupIndex Value = "-1"/>
	</PhysicBody>
	
	<Node Name = "Passive" Type = "Sprite">
		<ImageMap Name = "PplChain"/>
		<CenterAlign Value = "1"/>
	</Node>
	
</Entity>
'''

pepelaz["Chassis"] = '''
<Entity>
	
	<PhysicBody>
		<Shape>
			<Point Value = "-10; -10"/>
			<Point Value = "10; -10"/>
			<Point Value = "10; 10"/>
			<Point Value = "-10; 10"/>
		</Shape>
		<Density Value = "0.001"/>
		<Friction Value = "0.8"/>
		<Restitution Value = "0.5"/>
		<GroupIndex Value = "-1"/>
	</PhysicBody>w
	
	<Node Name = "Passive" Type = "Sprite">
		<CenterAlign Value = "1"/>
	</Node>
	
</Entity>
'''

pepelaz["Hook"] = '''
<Entity>
	
	<PhysicBody>
		<Shape>
			<Point Value = "-15; -14"/>
			<Point Value = "15; -14"/>
			<Point Value = "15; 14"/>
			<Point Value = "-15; 14"/>
		</Shape>
		<Density Value = "0.001"/>
		<Friction Value = "1"/>
		<Restitution Value = "0"/>
		<GroupIndex Value = "-1"/>
		<IsBullet Value = "1"/>
	</PhysicBody>
	
	<Node Name = "Passive" Type = "Sprite">
		<ImageMap Name = "PplHook"/>
		<CenterAlign Value = "1"/>
	</Node>
	
</Entity>
'''

pepelaz["Engine"] = '''
<Entity>
	
	<PhysicBody>
		<Shape>
			<Point Value = "0; 0"/>
			<Point Value = "10; 0"/>
			<Point Value = "10; 10"/>
			<Point Value = "0; 10"/>
		</Shape>
		<Density Value = "0.001"/>
		<Friction Value = "0.8"/>
		<Restitution Value = "0.5"/>
		<GroupIndex Value = "-1"/>
		<FixedRotation Value = "1"/>
	</PhysicBody>
	
	<Node Name = "Passive" Type = "Sprite">
		<Animation Name = "PplEngineAnimation"/>
		<CenterAlign Value = "1"/>
	</Node>
	
</Entity>
'''

pepelaz["NullObject"] = '''
<Entity>
	
	<PhysicBody>
		<Shape>
			<Point Value = "0; 0"/>
			<Point Value = "10; 0"/>
			<Point Value = "10; 10"/>
			<Point Value = "0; 10"/>
		</Shape>
		<Density Value = "0.001"/>
		<Friction Value = "0.8"/>
		<Restitution Value = "0.5"/>
		<CollisionMask Value = "0"/>
		<FixedRotation Value = "1"/>
	</PhysicBody>
	
	<Node Name = "Passive" Type = "Sprite">
		<ImageMap Name = "NullPixel"/>
	</Node>
	
</Entity>
'''