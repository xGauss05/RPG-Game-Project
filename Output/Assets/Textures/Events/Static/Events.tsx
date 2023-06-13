<?xml version="1.0" encoding="UTF-8"?>
<tileset version="1.10" tiledversion="1.10.1" name="Events" class="Tileset of Events" tilewidth="144" tileheight="144" tilecount="12" columns="1" objectalignment="topleft">
 <grid orientation="orthogonal" width="1" height="1"/>
 <tile id="2">
  <properties>
   <property name="Animation" type="class" propertytype="Event Animations"/>
  </properties>
  <image width="48" height="96" source="Aether.png"/>
 </tile>
 <tile id="3">
  <properties>
   <property name="Animation" type="class" propertytype="Event Animations"/>
  </properties>
  <image width="48" height="96" source="Balphogor.png"/>
 </tile>
 <tile id="5">
  <properties>
   <property name="Animation" type="class" propertytype="Event Animations"/>
  </properties>
  <image width="48" height="96" source="Captain.png"/>
 </tile>
 <tile id="6">
  <properties>
   <property name="Animation" type="class" propertytype="Event Animations"/>
  </properties>
  <image width="48" height="96" source="NPC01.png"/>
 </tile>
 <tile id="8">
  <properties>
   <property name="Animation" type="class" propertytype="Event Animations"/>
  </properties>
  <image width="48" height="96" source="Tycoon.png"/>
 </tile>
 <tile id="7">
  <properties>
   <property name="Animation" type="class" propertytype="Event Animations">
    <properties>
     <property name="Off" type="class" propertytype="Animation">
      <properties>
       <property name="Path" type="file" value="../Torch_Off.png"/>
      </properties>
     </property>
     <property name="On" type="class" propertytype="Animation">
      <properties>
       <property name="Loop" type="bool" value="true"/>
       <property name="LoopTimer" type="int" value="200"/>
       <property name="Path" type="file" value="../Torch_On.png"/>
      </properties>
     </property>
    </properties>
   </property>
  </properties>
  <image width="48" height="96" source="Torch.png"/>
 </tile>
 <tile id="4">
  <properties>
   <property name="Animation" type="class" propertytype="Event Animations">
    <properties>
     <property name="Off" type="class" propertytype="Animation">
      <properties>
       <property name="Path" type="file" value="../Chest_Off.png"/>
      </properties>
     </property>
     <property name="On" type="class" propertytype="Animation">
      <properties>
       <property name="Path" type="file" value="../Chest_On.png"/>
      </properties>
     </property>
     <property name="Triggered" type="class" propertytype="Animation">
      <properties>
       <property name="Loop" type="bool" value="true"/>
       <property name="LoopTimer" type="int" value="200"/>
       <property name="Path" type="file" value="../Chest_Triggered.png"/>
      </properties>
     </property>
    </properties>
   </property>
  </properties>
  <image width="48" height="48" source="Chest.png"/>
 </tile>
 <tile id="10">
  <properties>
   <property name="Animation" type="class" propertytype="Event Animations"/>
  </properties>
  <image width="48" height="48" source="Sign_Small.png"/>
 </tile>
 <tile id="11">
  <properties>
   <property name="Animation" type="class" propertytype="Event Animations"/>
  </properties>
  <image width="48" height="144" source="Sign_Tall.png"/>
 </tile>
 <tile id="13">
  <properties>
   <property name="Animation" type="class" propertytype="Event Animations"/>
  </properties>
  <image width="96" height="96" source="Sign_Big.png"/>
 </tile>
 <tile id="14">
  <image width="144" height="96" source="Door_Off.png"/>
 </tile>
 <tile id="15">
  <properties>
   <property name="Animation" type="class" propertytype="Event Animations"/>
  </properties>
  <image width="48" height="96" source="../Sayori.png"/>
 </tile>
</tileset>
