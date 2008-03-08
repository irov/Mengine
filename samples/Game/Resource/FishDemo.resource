<DataBlock>

	<Resource Name = "BlueActive" Type = "ResourceImageCell">
		<File Path = "Images/GO01_Active.png"/>
		<Cell X = "6" Y = "1"/>
	</Resource>

	<Resource Name = "SimpleAnimation" Type = "ResourceImageCell" >
		<File Path = "Images/grid1.png" />
		<Filter Value = "4" />
		<Cell X = "4" Y = "4" />
	</Resource>	

	<Resource Name = "grid3" Type = "ResourceImageCell" >
		<File Path = "Images/grid3.png" />
		<Filter Value = "4" />
		<Cell X = "4" Y = "4" />
	</Resource>	

	<Resource Name = "Water_01" Type = "ResourceImageDefault" >
		<File Path = "Images/Water/water_01.png" />
	</Resource>
	
	<Resource Name = "ReefValun_0N" Type = "ResourceImageDefault" >
		<File Path = "Images/Reef/1st_plan/testvalun_01.png" />
	</Resource>

	<Resource Name = "ReefValun_01" Type = "ResourceImageDefault" >
		<File Path = "Images/Reef/1st_plan/valun_01.png" />
	</Resource>

	<Resource Name = "ReefValun_02" Type = "ResourceImageDefault" >
		<File Path = "Images/Reef/1st_plan/valun_02.png" />
	</Resource>

	<Resource Name = "ReefValun_03" Type = "ResourceImageDefault" >
		<File Path = "Images/Reef/1st_plan/valun_03.png" />
	</Resource>

	<Resource Name = "ReefKamen_01" Type = "ResourceImageDefault" >
		<File Path = "Images/Reef/2nd_plan/kamen_01.png" />
	</Resource>

	<Resource Name = "ReefKamen_02" Type = "ResourceImageDefault" >
		<File Path = "Images/Reef/2nd_plan/kamen_02.png" />
	</Resource>

	<Resource Name = "ReefKamen_03" Type = "ResourceImageDefault" >
		<File Path = "Images/Reef/2nd_plan/kamen_03.png" />
	</Resource>

	<Resource Name = "Arial12" Type = "ResourceFont">
     		<File Path = "Fonts/ArialTest.fontdef"/>
	</Resource>

	<Resource Name = "ArialTest" Type = "ResourceFont">
     		<File Path = "Fonts/Arial-28pt.fontdef"/>
	</Resource>

	<Resource Name = "ResourcePlayListTest1" Type = "ResourcePlaylist">
		<File Path = "TestPlaylist.xml"/>
	</Resource>

	<Resource Name = "ResourceEmitterTest1" Type = "ResourceEmitterContainer">
		<File Path = "meduse.ptc"/>
		<Folder Path = "Particles/"/>
	</Resource>


	<Resource Name = "ResourceBoxGeom" Type = "ResourcePhysicBoxGeometry">
		<Width Value = "1"/>
		<Height Value = "1"/>
		<Depth Value = "1"/>
	</Resource>

	<Resource Name = "ResourceBarrelGeometry" Type = "ResourcePhysicConvexGeometry">
		<ConvexGeometryFile Name = "ZombieTest/barrel.bin"/>
	</Resource>

	<Resource Name = "ResourceLevelGeometry" Type = "ResourcePhysicConcaveGeometry">
		<ConcaveGeometryFile Name = "ZombieTest/cooked.bin"/>
	</Resource>

	<Resource Name = "TestSkeleton" Type = "ResourceSkeleton" >
		<Bones>
			<Bone Name = "1" />
			<Bone Name = "2" />
			<Bone Name = "3" />
		</Bones>
		<Animations>
			<Animation Name = "Idle" />
			<Animation Name = "Walk" />
			<Animation Name = "Die" />
		</Animations>
	</Resource>

	<Resource Name = "TestMesh" Type = "ResourceMesh" >
		<Mesh Name = "test.mesh" />
	</Resource>

	<Resource Name = "TestBarrel" Type = "ResourceMesh" >
		<Mesh Name = "barrel.mesh" />
	</Resource>

	<Resource Name = "TestRobotSkeleton" Type = "ResourceSkeleton" >
		<Bones>
			<Bone Name = "1" />
			<Bone Name = "2" />
			<Bone Name = "3" />
		</Bones>
		<Animations>
			<Animation Name = "Idle" />
			<Animation Name = "Walk" />
			<Animation Name = "Die" />
		</Animations>
	</Resource>

	<Resource Name = "TestRobotMesh" Type = "ResourceMesh" >
		<Mesh Name = "ZombieTest/robot.mesh" />
	</Resource>

	<Resource Name = "TestZombie" Type = "ResourceMesh" >
		<Mesh Name = "ZombieTest/Cylinder04.mesh" />
	</Resource>

	<Resource Name = "TestCap" Type = "ResourceCapsuleController" >
		<Radius Value = "1" />
		<Height Value = "1" />
	</Resource>

</DataBlock>
