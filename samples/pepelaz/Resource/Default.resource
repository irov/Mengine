<DataBlock>

	<!-- *************** Cursor *********************- -->
	<Resource Name = "default" Type = "ResourceImageDefault" >
		<File Path = "Game/Arrows/Default/Default.png" />
	</Resource>
	
	<Resource Name = "LevelBack" Type = "ResourceImageDefault" >
		<File Path = "Game/Art/Levels/TestLevel/Back.png" />
	</Resource>
	
	<Resource Name = "LevelSecondPlan11" Type = "ResourceImageDefault" >
		<File Path = "Game/Art/Levels/TestLevel/SecondPlan_11.png" />
	</Resource>
	
	<Resource Name = "LevelSecondPlan12" Type = "ResourceImageDefault" >
		<File Path = "Game/Art/Levels/TestLevel/SecondPlan_12.png" />
	</Resource>
	
	<Resource Name = "LevelSecondPlan13" Type = "ResourceImageDefault" >
		<File Path = "Game/Art/Levels/TestLevel/SecondPlan_13.png" />
	</Resource>
	
	<Resource Name = "LevelSecondPlan21" Type = "ResourceImageDefault" >
		<File Path = "Game/Art/Levels/TestLevel/SecondPlan_21.png" />
	</Resource>
	
	<Resource Name = "LevelSecondPlan22" Type = "ResourceImageDefault" >
		<File Path = "Game/Art/Levels/TestLevel/SecondPlan_22.png" />
	</Resource>
	
	<Resource Name = "LevelSecondPlan23" Type = "ResourceImageDefault" >
		<File Path = "Game/Art/Levels/TestLevel/SecondPlan_23.png" />
	</Resource>
	
	<Resource Name = "LevelSecondPlan31" Type = "ResourceImageDefault" >
		<File Path = "Game/Art/Levels/TestLevel/SecondPlan_31.png" />
	</Resource>
	
	<Resource Name = "LevelSecondPlan32" Type = "ResourceImageDefault" >
		<File Path = "Game/Art/Levels/TestLevel/SecondPlan_32.png" />
	</Resource>
	
	<Resource Name = "LevelSecondPlan33" Type = "ResourceImageDefault" >
		<File Path = "Game/Art/Levels/TestLevel/SecondPlan_33.png" />
	</Resource>
	
	<!-- **************** Effects ********************* -->
	<Resource Name = "NullPixel" Type = "ResourceImageDefault" >
		<File Path = "Game/Art/nullPixel.png" />
	</Resource>
	
	<!-- **************** PEPELAZ ********************* -->
	<Resource Name = "PplTorso" Type = "ResourceImageDefault" >
		<File Path = "Game/Art/Objects/Pepelaz/korpys.png" />
	</Resource>
	
	<Resource Name = "PplScrewUp" Type = "ResourceImageDefault" >
		<File Path = "Game/Art/Objects/Pepelaz/vint_verh1.png" />
		<File Path = "Game/Art/Objects/Pepelaz/vint_verh2.png" />
	</Resource>
	
	<Resource Name = "PplScrewDown" Type = "ResourceImageDefault" >
		<File Path = "Game/Art/Objects/Pepelaz/vint_vniz1.png" />
		<File Path = "Game/Art/Objects/Pepelaz/vint_vniz2.png" />
	</Resource>
	
	<Resource Name = "PplScrewAnimation" Type = "ResourceAnimation">
		<Sequences>
			<Sequence Index = "0" Delay = "50"/>
			<Sequence Index = "1" Delay = "50"/>
		</Sequences>
	</Resource>
	
	<Resource Name = "PplLeftChassis" Type = "ResourceImageDefault" >
		<File Path = "Game/Art/Objects/Pepelaz/levua9_lapa.png" />
	</Resource>
	
	<Resource Name = "PplRightChassis" Type = "ResourceImageDefault" >
		<File Path = "Game/Art/Objects/Pepelaz/prava9_lapa.png" />
	</Resource>
	
	<Resource Name = "PplLeftEngine" Type = "ResourceImageDefault" >
		<File Path = "Game/Art/Objects/Pepelaz/levui_dvig1.png" />
		<!--
		<File Path = "Game/Art/Objects/Pepelaz/levui_dvig2.png" />
		-->
	</Resource>
	
	<Resource Name = "PplRightEngine" Type = "ResourceImageDefault" >
		<File Path = "Game/Art/Objects/Pepelaz/pravui_dvig1.png" />
		<!--
		<File Path = "Game/Art/Objects/Pepelaz/pravui_dvig2.png" />
		-->
	</Resource>
	
	<Resource Name = "PplChain" Type = "ResourceImageDefault" >
		<File Path = "Game/Art/Objects/Pepelaz/ChainSegment.png" />
	</Resource>
	
	<Resource Name = "PplHook" Type = "ResourceImageDefault" >
		<File Path = "Game/Art/Objects/Pepelaz/Hook.png" />
	</Resource>
	
	<Resource Name = "PplEngineAnimation" Type = "ResourceAnimation">
		<Sequences>
			<Sequence Index = "0" Delay = "20"/>
			<Sequence Index = "1" Delay = "20"/>
		</Sequences>
	</Resource>
	
	<Resource Name = "PplAim" Type = "ResourceImageDefault" >
		<File Path = "Game/Art/Objects/aim.png" />
	</Resource>
	
	<Resource Name = "Box" Type = "ResourceImageDefault" >
		<File Path = "Game/Art/Objects/Box/Box.png" />
	</Resource>
	
	<Resource Name = "HealthIndicator" Type = "ResourceImageDefault">
		<File Path = "Game/Art/Objects/HealthIndicator.png"/>
	</Resource>

	<Resource Name = "TestTileSet" Type = "ResourceTileSet" >
		<TileSet File = "Game/Art/grid.png" />
		<Tiles Value = "2" />
	</Resource>

	<Resource Name = "TestTileMap" Type = "ResourceTileMap" >
		<TileMap File = "Game/Art/tilemap.txt" />
		<ResourceTileSet Name = "TestTileSet" />
	</Resource>

	<!-- ********************** PARTICLES ******************* -->
	<Resource Name = "Particles" Type = "ResourceEmitterContainer">
  	<File Path = "Game/Particles/particles.ptc"/>
  	<Folder Path = "Game/Particles/"/>
	</Resource>

</DataBlock>