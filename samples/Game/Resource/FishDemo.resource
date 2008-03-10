<DataBlock>

	<Resource Name = "ResourceBoxGeom" Type = "ResourcePhysicBoxGeometry">
		<Width Value = "1"/>
		<Height Value = "1"/>
		<Depth Value = "1"/>
	</Resource>

	<Resource Name = "ResourceBarrelGeometry" Type = "ResourcePhysicConvexGeometry">
		<ConvexGeometryFile Name = "ZombieTest/barrel.bin"/>
	</Resource>

	<Resource Name = "ResourceLevelGeometry" Type = "ResourcePhysicConcaveGeometry">
		<ConcaveGeometryFile Name = "ZombieTest/level.bin"/>
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

	<Resource Name = "TestLevel" Type = "ResourceMesh" >
		<Mesh Name = "level.mesh" />
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

	<Resource Name = "TestZombie" Type = "ResourceMesh" >
		<Mesh Name = "z_def.mesh" />
	</Resource>

	<Resource Name = "TestCap" Type = "ResourceCapsuleController" >
		<Radius Value = "1" />
		<Height Value = "1" />
	</Resource>

</DataBlock>
