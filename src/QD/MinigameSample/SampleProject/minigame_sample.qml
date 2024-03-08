<?xml version="1.0" encoding="WINDOWS-1251"?>
<qd_script>
	<screen_size>800 600</screen_size>
	<text_set ID="0" pos_2d="320 240" screen_size="640 360"/>
	<mouse_object name="Мышь" default_cursors="0 0 0 0 0 0">
		<pos_3d>0. 0. 0.</pos_3d>
		<flag>32</flag>
		<bound>10. 10. 10.</bound>
		<default_pos>0. 0. 0.</default_pos>
	</mouse_object>
	<scene name="Minigame" game_name="MinigameSample">
		<animation name="ball" flags="384" animation_file="Resource\ball.qda"/>
		<animation name="board" flags="384" animation_file="Resource\board.qda"/>
		<animation name="brick0" flags="384" animation_file="Resource\brick0.qda"/>
		<animation name="brick1" flags="384" animation_file="Resource\brick1.qda"/>
		<animation name="brick2" flags="384" animation_file="Resource\brick2.qda"/>
		<animation name="back" flags="384" animation_file="Resource\back.qda"/>
		<camera camera_grid_size="46 46">
			<camera_cell_size>32 32</camera_cell_size>
			<pos_3d>0 0 300.</pos_3d>
			<camera_focus>6000.</camera_focus>
			<camera_angles>1. 0. 0.</camera_angles>
			<camera_screen_size>800 600</camera_screen_size>
			<camera_screen_offset>-80 -61</camera_screen_offset>
			<camera_screen_center>320 240</camera_screen_center>
			<camera_grid_center>0. 0. 0.</camera_grid_center>
			<camera_scale_pow>1.</camera_scale_pow>
			<camera_scale_z_offset>0.</camera_scale_z_offset>
		</camera>
		<animated_object name="шарик">
			<pos_3d>-347.13956 413.3905 0.</pos_3d>
			<object_state_static name="ball" flags="256" bound="20. 18. 10.">
				<animation_info animation="ball"/>
			</object_state_static>
			<bound>10. 10. 10.</bound>
			<default_pos>-347.13956 413.3905 0.</default_pos>
		</animated_object>
		<animated_object name="ракетка">
			<pos_3d>-395.97217 413.95166 0.</pos_3d>
			<object_state_static name="board" flags="256" bound="64. 14. 10.">
				<animation_info animation="board"/>
			</object_state_static>
			<bound>10. 10. 10.</bound>
			<default_pos>-395.97217 413.95166 0.</default_pos>
		</animated_object>
		<animated_object name="кирпич">
			<pos_3d>-471.52496 414.35165 0.</pos_3d>
			<object_state_static name="01" flags="264" bound="64. 19. 20.">
				<animation_info animation="brick0"/>
			</object_state_static>
			<object_state_static name="0" flags="256" bound="64. 19. 20.">
				<animation_info animation="brick0"/>
			</object_state_static>
			<object_state_static name="1" flags="256" bound="64. 19. 10.">
				<animation_info animation="brick1"/>
			</object_state_static>
			<object_state_static name="2" flags="256" bound="64. 19. 10.">
				<animation_info animation="brick2"/>
			</object_state_static>
			<bound>10. 10. 10.</bound>
			<default_pos>-471.52496 414.35165 0.</default_pos>
		</animated_object>
		<animated_object name="фон">
			<pos_3d>-84. 64. 0.</pos_3d>
			<object_state_static name="back" flags="256" bound="734. 522. 1.">
				<animation_info animation="back"/>
			</object_state_static>
			<bound>10. 10. 10.</bound>
			<default_pos>-84. 64. 0.</default_pos>
		</animated_object>
	</scene>
	<minigame name="MinigameSample" config_file="DLL\minigame_sample_config.ini" dll_name="DLL\MinigameSample.dll">
		<minigame_config_prm name="ball_object_name" type="0" comment="имя объекта-шарика">шарик</minigame_config_prm>
		<minigame_config_prm name="board_object_name" type="0" comment="имя объекта-ракетки">ракетка</minigame_config_prm>
		<minigame_config_prm name="field_object_name" type="0" comment="имя объекта-поля">фон</minigame_config_prm>
		<minigame_config_prm name="ball_speed" type="0" comment="начальная скорость шарика">100 200</minigame_config_prm>
		<minigame_config_prm name="ball_speed_rnd" type="0" comment="границы случайной добавки к начальной скорости шарика">10 10</minigame_config_prm>
		<minigame_config_prm name="brick_object" type="2" comment="объекты, из которых строится игровое поле" size="200">кирпич</minigame_config_prm>
		<minigame_config_prm name="ball_object" type="2" comment="шарики, которые выводятся как запасные" size="50">шарик</minigame_config_prm>
		<minigame_config_prm name="map_file_name" type="0" comment="файл расположения блоков">.\DLL\smile.map</minigame_config_prm>
	</minigame>
	<interface>
	</interface>
</qd_script>
