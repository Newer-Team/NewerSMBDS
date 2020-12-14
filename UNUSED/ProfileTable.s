repl_02004FF8_main:
repl_020235BC_main:
	LDR	  R1, =ProfileTable
	BX    LR
	
.data
.balign 4
ProfileTable:
	.long 0x020CD0E0	@ Boot
	.long 0x0203967C	@ Connect
	.long 0x020CD298	@ Debug
	.long 0x020C6D28    @ Stage
	.long 0x020DA814    @ Main Menu
	.long 0x0215B6C0	@ MvsL 1
	.long 0x0215BDEC    @ MvsL Opponent
	.long 0x020E5820    @ Debug Save
	.long 0x020E5878    @ Unused World Select
	.long 0x020E5CC0	@ World Map Scene
	.long 0x0215BB9C	@ MvsL 2
	.long 0x0203967C    @ Corrupted File
	.long 0x020E9B88	@ Ending
	.long 0x0216CCA0	@ Course In
	.long 0x020CCCC0	@ Game Over
	.long 0x02151280	@ MvsL Course In
	.long 0x020CC5E0	@ Debug Sound Test 
	.long 0x020CC620	@ Debug Key Test
	.long 0x021277D4
	.long 0x02126B7C
	.long 0x02170820	@ View Enter Event Controller
	.long 0x02128458	@ Player
	.long 0x02126BE0
	.long 0x021439C8	@ Pipe Piranha Plant Upward
	.long 0x021439D4	@ Pipe Piranha Plant ?
	.long 0x021439E0	@ Pipe Piranha Plant ?
	.long 0x021439EC	@ Pipe Piranha Plant ?
	.long 0x02151FB0
	.long 0x0213EDF8	@ Cheep Cheep
	.long 0x02176770	@ Giant Cheep Cheep
	.long 0x0214180C	@ Cheep Cheep with Coin Trail
	.long 0x02124350
	.long 0x0212435C
	.long 0x021331C8	@ VS Battle Star
	.long 0x021331D4    @ VS Battle Star ?
	.long 0x02152460	@ Bob-omb
	.long 0x021320A0	@ Goal
	.long 0x0213F6D0	@ Thwomp
	.long 0x0213F6DC	@ Giant Thwomp
	.long 0x02178AF0	@ Lakitu
	.long 0x021526C0	@ Spiny
	.long 0x02179378	@ Boo
	.long 0x02179384	@ Balloon Boo
	.long 0x02137458	@ Monty Tank
	.long 0x02173098	@ Chain Chomp Log
	.long 0x02175750	@ Chain Chomp
	.long 0x0213ED38	@ Fire Snake
	.long 0x0218978C	@ Flip Fence
	.long 0x02189780	@ Large Flip Fence
	.long 0x02125088	@ Zoom
	.long 0x0217F75C	@ Bubbles
	.long 0x0217F738	@ Bubbles 2?
	.long 0x0217F750	@ Bubbles 3?
	.long 0x0217F744	@ Bubbles 4?
	.long 0x02179180	@ Buzzy Beetle
	.long 0x0213F450	@ Dry Bones
	.long 0x0213F45C	@ Giant Dry Bones
	.long 0x02148A48	@ Lava Bubble
	.long 0x02151C38	@ Bullet Bill
	.long 0x02178948	@ Lakitu Spawner
	.long 0x021416B0	@ Cheep Cheep Spawner
	.long 0x0217B4C0	@ Shark Spawner
	.long 0x0216D60C	@ Mega Mario Spawn Chance Increaser
	.long 0x0217D810	@ Scuttlebug
	.long 0x0213C548	@ Pipe Generator
	.long 0x0213F928	@ Firebar
	.long 0x021251F8	@ Coin in Bubble
	.long 0x0216D01C
	.long 0x02175478	@ Hammer Brother
	.long 0x0216E16C	@ Overworld Hammer Brother
	.long 0x02175818	@ Sledge Brother
	.long 0x02170538	@ Donut Lift
	.long 0x0217067C
	.long 0x021499A8	@ Whomp
	.long 0x0217B390	@ Shark
	.long 0x0217C1C0	@ Unagi the Eel
	.long 0x0217C1CC	@ Giant Unagi the Eel
	.long 0x021321EC	@ Arrow Sign
	.long 0x0213BBEC
	.long 0x02148284	@ Dorrie
	.long 0x0216CEC8	@ Red Coin
	.long 0x02143AF0	@ Skeeter
	.long 0x0218A518	@ Expandable Block
	.long 0x021263FC	@ Goomba
	.long 0x02126408	@ Paragoomba
	.long 0x02126414	@ Microgoomba
	.long 0x0216DD40	@ Event Block
	.long 0x0216D284	@ Warp Entrance
	.long 0x02175484	@ Fire Brother
	.long 0x02175490	@ Boomerang Brother
	.long 0x0213CD18	@ Swooper
	.long 0x02143770	@ Snailicorn
	.long 0x0216D4DC	@ Physics-Affected Coin
	.long 0x021469B0	@ Boss Key
	.long 0x021261B4	@ Koopa Troopa
	.long 0x021261C0	@ Koopa Paratroopa
	.long 0x02145200	@ Fence Koopa Troopa
	.long 0x02133D08	
	.long 0x0213EBF0	@ Pokey
	.long 0x0217B714	@ Ground Piranha Plant
	.long 0x0217B708	@ Giant Ground Piranha Plant
	.long 0x0217B884	@ Grounded Venus Fire Trap
	.long 0x0217B890	@ Giant Grounded Venus Fire Trap
	.long 0x0218DFA8	@ Spike Balloon
	.long 0x0218DE90	@ Spike Ball Activator
	.long 0x02185478	@ Slow Purple Tilting Mushroom
	.long 0x0213ED44	@ Fire Snake
	.long 0x02177488	@ Flame Chomp
	.long 0x02177620	
	.long 0x02177740
	.long 0x02179420	@ Amp
	.long 0x021446D0	@ Wiggler
	.long 0x02148548	@ Cheep Chomp
	.long 0x021413D0	@ Bowser Jr.
	.long 0x02146720
	.long 0x02146870	@ Peach
	.long 0x02148B08	@ Giant Bowser Pot
	.long 0x0217D6E0	@ Hanging Scuttlebug
	.long 0x0217D968	@ Money Bag
	.long 0x02134728	@ Petey Piranha
	.long 0x0218AEF0	@ Broozer
	.long 0x02170130
	.long 0x02143C18
	.long 0x021526CC	@ Spiny
	.long 0x0218A800	@ Bounce Ball
	.long 0x0218DEE0	@ Arrow
	.long 0x0217C684	@ Squiggler
	.long 0x02179880	@ Splunkin
	.long 0x02133A88	@ Mummy Pokey
	.long 0x02133BD0	@ Lakithunder
	.long 0x021876D8	@ Spinning Ball on Chain
	.long 0x0213BA78	@ Bowser 
	.long 0x0213BA84 	@ Dry Bowser
	.long 0x0213BA90	@ Final Bowser
	.long 0x0218F3E8	@ Mushroom House Block
	.long 0x021324F8	@ Cheep Skipper
	.long 0x021483B0	@ Jumping Cheep Skipper
	.long 0x0217082C	@ AND Event Controller
	.long 0x0203967C	@ OR Event Controller
	.long 0x021707FC	@ RANDOM Event Controller
	.long 0x02170814	@ CHAINER Event Controller
	.long 0x02170838	@ ZONE Event Controller
	.long 0x02170808	@ MULTICHAINER Event Controller
	.long 0x02141BB8	@ Blooper
	.long 0x02141BA0	@ Blooper Nanny?
	.long 0x02141BAC	@ Tiny Blooper?
	.long 0x02141CE4
	.long 0x021792C0	@ Spike Top
	.long 0x02143368	@ Crowber
	.long 0x0213D878	@ Kab-omb
	.long 0x02173680	@ Spike Bass
	.long 0x0216CD90	@ Red Ring
	.long 0x021795D0	@ Ghost House Hand
	.long 0x02179710	@ Invisible Lava Bubble
	.long 0x02143490	@ Crowber Spawner
	.long 0x0217BAA8	@ Snow Spike
	.long 0x0217B928	@ Snowball
	.long 0x021737A8	@ Spike Bass
	.long 0x0216FF64	@ Vertical Midway Point
	.long 0x0216FF58	@ Midway Point
	.long 0x0216E2B0
	.long 0x02125904	@ Vertical Moving Platform
	.long 0x02125A4C	@ Horizontal Moving Platform
	.long 0x02125A58
	.long 0x02189650	@ Spinning Log
	.long 0x02125DD8	@ Vertical Moving Stone Block
	.long 0x02125F34	@ Horizontal Moving Stone Block
	.long 0x02125F28	
	.long 0x02133090	@ Horizontal Moving Stone Block With Spikes
	.long 0x021487E8	@ Hanging Metal Platform
	.long 0x02148920	@ Falling Rock Platform
	.long 0x0217FBA0	@ Seesaw
	.long 0x0218AD60 	@ Scale
	.long 0x0217FA50	@ Fall when touched Platform
	.long 0x02182440	@ Move when touched Platform
	.long 0x0217EB60	@ Platform Spinner
	.long 0x02126E2C	
	.long 0x021332F4	@ Vertical Platform Generator
	.long 0x02133414
	.long 0x021892D8	@ Spinning Rectangle Platform
	.long 0x021893F8	@ Spinning Triangle Platform
	.long 0x02189518	@ Spinning Square Platform
	.long 0x02132364	@ Line Controlled Platform
	.long 0x02188040    @ Manual Moving Wheel
	.long 0x02188A98	@ Swinging Pole on Line
	.long 0x0218E0D0	@ Giant Spike Ball
	.long 0x021821A0	@ Giant Platform Spinner
	.long 0x02189870	@ Manhole
	.long 0x02188970	@ Bump From Below Platform
	.long 0x02184C88	@ Mushroom Platform
	.long 0x02184ED8	@ Purple Tilt Mushroom
	.long 0x02185088	@ Up and Down Mushroom
	.long 0x02147FF0	@ Rising Green Mushroom
	.long 0x02185230	@ Stretching Mushroom
	.long 0x02185350	@ Bouncing Mushroom
	.long 0x02184DB8	@ Horizontal Moving Mushroom
	.long 0x0218A870	@ Rope
	.long 0x02187EE0	@ Vine
	.long 0x021822E0	@ Manual Control Platform
	.long 0x02190034	@ Ground Pound Ghost Goo
	.long 0x0218EFC0	@ Falling Snow
	.long 0x021734E0	@ Cloud Platform
	.long 0x02127478	@ Pipe Cannon
	.long 0x02174868	
	.long 0x02174718	@ Swelling Pipe
	.long 0x02174990
	.long 0x0216D3B4	@ Tile Manipulator
	.long 0x0217E334	@ Waving Ghost House Goo
	.long 0x02188208	@ Barrel
	.long 0x02182578	@ Walljump Platform
	.long 0x0218FEEC	@ Rotating Carrythrough Platform
	.long 0x0212704C	@ Smashed Pipe
	.long 0x0218F168	@ Falling Snow
	.long 0x0218E0B0	@ Gate
	.long 0x02182978	@ Snake Blocks
	.long 0x0218FD88	@ Ghost House Lift
	.long 0x0218DEC8	@ Skewer 1
	.long 0x0218DEA4	@ Skewer 2
	.long 0x0218DEB0	@ Skewer 3
	.long 0x0218DEBC	@ Skewer 4
	.long 0x0218DE98	@ Skewer 5
	.long 0x0218DE8C	@ Skewer 6
	.long 0x0218DE74	@ Skewer 7
	.long 0x0218DE80	@ Skewer 8
	.long 0x0218E180	@ Giant Wiggler
	.long 0x0217EC88	@ Tweester
	.long 0x0217F89C	@ Whirlpool
	.long 0x0217F600	@ Floating Log
	.long 0x02182050	@ Pendulum Platform
	.long 0x021864D0	
	.long 0x02181F00	@ Drawbridge Platform
	.long 0x02188830	@ Swelling Ground
	.long 0x02189718	@ Tightrope
	.long 0x02186370	@ Bouncing Bricks
	.long 0x02187468	@ Small Flamethrower
	.long 0x021875B0	@ Flamethrower
	.long 0x02132494
	.long 0x0216D154	@ Springboard
	.long 0x02151FBC
	.long 0x021330E8	@ Mega Goomba
	.long 0x02124014	@ Activated Brick Block
	.long 0x021245A8
	.long 0x02171020	@ Switch
	.long 0x0217102C	@ Brick Block with Switch
	.long 0x02171038	@ P-Switch
	.long 0x02171044	@ Brick Block with P-Switch
	.long 0x02171050	@ Red Switch
	.long 0x0217105C	@ Brick Block with Red Switch
	.long 0x02151D74	@ Bullet Bill Cannon
	.long 0x02151D68	@ Banzai Bill Cannon
	.long 0x02189478	@ Hanging Block
	.long 0x0216D9AC	@ Rotating Spike Block
	.long 0x0216D9B8	@ Rotating Spike Block 2
	.long 0x0216D9C4	@ Rotating Spike Block 3
	.long 0x02125348	@ Spinning Jumpboard
	.long 0x0212533C	@ Spinning Jumpboard 2
	.long 0x02125354	@ Spinning Jumpboard 3
	.long 0x0216D744	@ Roulette Block
	.long 0x021895D4	@ Hanging Block
	.long 0x02178438	@ Blockhopper
	.long 0x0216DEC4 	@ Flying Block
	.long 0x0216E044	@ Red Flying Block
	.long 0x02152238	@ Rotating Bullet Bill Cannon
	.long 0x0218FC30	@ Event Activated Door
	.long 0x0218F550	@ Mushroom House Block
	.long 0x0213BD2C	@ Bowser Battle Switch
	.long 0x02146AD8	@ Boss Key Location
	.long 0x021291BC	
	.long 0x021271DC	@ Invisible Coin Block
	.long 0x0217FCF0	@ World Warp Cannon
	.long 0x021246C8	
	.long 0x021246D4
	.long 0x02124800	@ Stick to Bottom Activator
	.long 0x021247F4
	.long 0x02124944	@ Vertical Camera Limit
	.long 0x02124938
	.long 0x02124B84	@ Autoscroll
	.long 0x02124CA4	@ Horizontal Camera Limit
	.long 0x021702A8	@ Water
	.long 0x021702B4	@ Poison Water
	.long 0x02170290	@ Lava
	.long 0x0217029C	@ Junglekusa
	.long 0x02124A64	@ Horizontal Camera Offset
	.long 0x0212492C	@ In Air Vertical Camera Limit
	.long 0x02124920
	.long 0x02170400	@ Loop Creator
	.long 0x021703F4	
	.long 0x02124E94
	.long 0x02124EDC
	.long 0x02124EA0	@ Fog
	.long 0x02124EAC
	.long 0x02124EC4
	.long 0x02124E7C
	.long 0x02124E34
	.long 0x02124ED0
	.long 0x02124E40
	.long 0x02124EB8
	.long 0x02124E58
	.long 0x02124E64
	.long 0x02124E70
	.long 0x0203967C
	.long 0x02141A50	@ Seaweed
	.long 0x02170084
	.long 0x020C9388
	.long 0x0215C67C
	.long 0x020DABB4
	.long 0x020DAC58
	.long 0x020DACDC
	.long 0x020DAEA0
	.long 0x020DAF4C
	.long 0x020DAFD0
	.long 0x020DB04C
	.long 0x020DB0B0
	.long 0x020DB134
	.long 0x020EE2C8
	.long 0x020CCD30
	.long 0x02123C80	@ Camera
	.long 0x020E59A8	@ Unused World Select model
	.long 0x020E58D0
	.long 0x020E9520	@ World Map Player
	.long 0x020E98CC	@ World Map Bowser Jr.
	.long 0x020E9990	
	.long 0x020E8BEC	@ World Map
	.long 0x020DAADC
	.long 0x020E9BF0
	.long 0x02123D58	@ Last Actor