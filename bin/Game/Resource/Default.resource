<DataBlock>

	<!- ************************************************ -->
	<!- Image for Background -->

	<Resource Name = "Back_01" Type = "ResourceImageDefault" >
		<File Path = "Game/Scenes/Buka/background/Kuzya_yard_right.png" />
	</Resource>

	<!- ************************************************ -->
	<!- Cal3d Test Animation -->

	<Resource Name = "Cal3dTest" Type = "ResourceAnimationCal3d" >
		<File Config = "Game/Animations/paladin/paladin.cfg" />
		<Sequences>
			<AnimInfo Index = "0" Delay = "5.0" Blend = "1.0" />
			<AnimInfo Index = "1" Delay = "5.0" Blend = "1.0" />
			<AnimInfo Index = "2" Delay = "5.0" Blend = "1.0" />
		</Sequences>	
		<HardPoints>
			<HardPoint Name = "Bip01 Tail" />
			<HardPoint Name = "Bip01 Tail1" />
		</HardPoints>
	</Resource>

	<Resource Name = "default" Type = "ResourceImageMNG" >
		<File Path = "Game/Arrows/Default/Default.mng" />
	</Resource>
		
	<Resource Name = "grid" Type = "ResourceImageCell" >
		<File Path = "Game/Images/grid.png" />
		<Filter Value = "4" />
		<Cell X = "4" Y = "4" />
	</Resource>	

	<Resource Name = "TestAtlas" Type = "ResourceImageSet" >
		<File Path = "Game/Images/grid.png" />
		<Image UV = "0.0;0.0;0.25;0.25" />
		<Image UV = "0.75;0.75;1.0;1.0" />
	</Resource>	

	<Resource Name = "BallRed" Type = "ResourceImageDefault" >
		<File Path = "Game/Images/BallRed.png" />
	</Resource>

	<Resource Name = "BallGreen" Type = "ResourceImageDefault" >
		<File Path = "Game/Images/BallGreen.png" />
	</Resource>

	<Resource Name = "BallBlue" Type = "ResourceImageDefault" >
		<File Path = "Game/Images/BallBlue.png" />
	</Resource>

	<Resource Name = "BallYellow" Type = "ResourceImageDefault" >
		<File Path = "Game/Images/BallYellow.png" />
	</Resource>

	<Resource Name = "BallVialet" Type = "ResourceImageDefault" >
		<File Path = "Game/Images/BallVialet.png" />
	</Resource>

	<Resource Name = "BallRedCNG" Type = "ResourceAnimation" >
		<ImageMap Name = "BallRed" />
		<Sequences>
			<Sequence Index = "1" Delay = "50" />
			<Sequence Index = "2" Delay = "50" />
			<Sequence Index = "3" Delay = "50" />
			<Sequence Index = "4" Delay = "50" />
			<Sequence Index = "14" Delay = "50" />
		</Sequences>
	</Resource>

</DataBlock>