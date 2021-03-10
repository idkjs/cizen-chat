
open Phx;
open !MessageMap;

let subtract = (a1, a2) => {
  let l2 = Array.to_list(a2);
  Array.to_list(a1)
  |> List.filter(e => !List.mem(e, l2))
  |> Array.of_list;
};

type ready_state = {
  id: string,
  name: string,
  socket: Socket.t,
  channel: Channel.t,
  rooms: Room.by_room_id,
  available: array(string), /* redundant? */
  entered: array(string),
  messages: Messages.by_room_id_t,
  text: string,
  selected: option(string)
};

type state =
  | Connecting
  | Ready(ready_state)
  | Error;

type action =
  | Connected(string, string, Socket.t, Channel.t)
  | RoomCreated(string, string, string)
  | RoomEnter(string)
  | RoomSelect(string)
  | ReceiveRoomSetting(string, string, string)
  | SendRoomSetting(option(string), option(string))
  | ReceiveAvatarProfile(string)
  | SendAvatarProfile
  | Receive(string, string, string)
  | UpdateText(string);

type action2 =
  | Connect
  | RoomCreate
  | Send;

let reducer = (state,action) =>
    switch (action) {
    // | Connect => ()

    | Connected(id, name, socket, channel) =>
      Ready({
        id,
        name,
        socket,
        channel,
        rooms: Room.RoomMap.empty,
        available: [||],
        entered: [||],
        messages: MessageMap.empty,
        text: "",
        selected: None
      });
    // | Send => {
    //   switch (state) {
    //   | Ready({ id, channel, text, selected }) =>
    //     switch (selected) {
    //     | Some(room) =>
    //       push("room:message", {"source": id, "room_id": room, "body": text}, channel) |> ignore;
    //       /* Loopback. Update self state */
    //       dispatch(Receive(id, room, text));
    //       dispatch(UpdateText(""));
    //     | None =>state
    //     }
    //   | _ =>state
    //   }};
    | Receive(source, room_id, body) =>
      switch (state) {
      | Ready({ messages } as state) =>
        (Ready({
          ...state,
          messages: Message.addMsg(source, room_id, body, messages)
        }))
      | _ => state
      }
    // | RoomCreate => {
    //   switch (state) {
    //   | Ready({ id, channel }) =>
    //     push("room:create", {"source": id}, channel)
    //       |> putReceive("ok", (res: Abstract.any) => {
    //         let { room_id, name, color }: Decode.setting = Decode.setting(res);
    //         dispatch(RoomCreated(room_id, name, color));
    //       })
    //       |> ignore;

    //   | _ => state
    //   }};
    | RoomCreated(room_id, name, color) =>
      switch (state) {
      | Ready({ rooms, available, entered } as state) =>
        (Ready({
          ...state,
          rooms: Room.upsertRoom(room_id, name, color, rooms),
          available: Room.uniqRooms(room_id, available),
          entered: Room.uniqRooms(room_id, entered),
          selected: Some(room_id)
        }))
      | _ => state
      }
    | RoomEnter(room_id) =>
      switch (state) {
      | Ready(state) =>
        push("room:enter", {"source": id, "room_id": room_id}, channel) |> ignore;
        (Ready({
          ...state,
          entered: Room.uniqRooms(room_id),
          selected: Some(room_id)
        }))
      | _ => state
      }
    | RoomSelect(room_id) =>
      switch (state) {
      | Ready({ selected } as state) =>
        (Ready({
          ...state,
          selected: Some(room_id)
        }))
      | _ => state
      }
    | ReceiveRoomSetting(room_id, name, color) =>
      switch (state) {
      | Ready({ rooms, available } as state) =>
        (Ready({
          ...state,
          rooms: Room.upsertRoom(room_id, name, color, rooms),
          available: Room.uniqRooms(room_id, available)
        }))
      | _ => state
      }
    | SendRoomSetting(name_opt, color_opt) => {
      switch (state) {
      | Ready({ id, channel, rooms, selected }) =>
        switch (selected) {
        | Some(room) =>
          let name = switch (name_opt) {
          | Some(name) => name
          | None => Room.getRoomName(room, rooms)
          };
          let color = switch (color_opt) {
          | Some(color) => color
          | None => Room.getRoomColor(room, rooms)
          };
          push("room:setting", {"source": id, "room_id": room, "name": name, "color": color}, channel) |> ignore;
          /* Loopback. Update self state */
          dispatch(ReceiveRoomSetting(room, name, color));
        | None => state
        }
      | _ => state
      }};
    | ReceiveAvatarProfile(name) =>
      switch (state) {
      | Ready(state) => (Ready({...state, name}))
      | _ => state
      }
    | SendAvatarProfile => {
      switch (state) {
      | Ready({ id, channel }) =>
        push("avatar:profile", {"source": id, "name": name}, channel) |> ignore;
        /* Loopback. Update self state */
        dispatch(ReceiveAvatarProfile(id, name));
      | _ => state
      }};
    | UpdateText(input) =>
      switch (state) {
      | Ready({ text } as state) =>
        (Ready({
          ...state,
          text: input
        }))
      | _ => state
      }
    };
[@react.component]
let make =() => {
let (state,dispatch) = React.useReducer(reducer,Connecting);
let handleSend = ()=> {
      switch (state) {
      | Ready({ id, channel, text, selected }) =>
        switch (selected) {
        | Some(room) =>
          push("room:message", {"source": id, "room_id": room, "body": text}, channel) |> ignore;
          /* Loopback. Update self state */
          dispatch(Receive(id, room, text));
          dispatch(UpdateText(""));
        | None =>state
        }
      | _ =>state
      }};
let handleConnected=(id, name, socket, channel) =>
      Ready({
        id,
        name,
        socket,
        channel,
        rooms: Room.RoomMap.empty,
        available: [||],
        entered: [||],
        messages: MessageMap.empty,
        text: "",
        selected: None
      });
let handleReceiveRoomSetting =(room_id, name, color) =>
      switch (state) {
      | Ready({ rooms, available } as state) =>
        setState(_=>Ready({
          ...state,
          rooms: Room.upsertRoom(room_id, name, color, rooms),
          available: Room.uniqRooms(room_id, available)
        }))
      | _ => ()
      };
let handleSendRoomSetting = (name_opt, color_opt) => {
      switch (state) {
      | Ready({ id, channel, rooms, selected }) =>
        switch (selected) {
        | Some(room) =>
          let name = switch (name_opt) {
          | Some(name) => name
          | None => Room.getRoomName(room, rooms)
          };
          let color = switch (color_opt) {
          | Some(color) => color
          | None => Room.getRoomColor(room, rooms)
          };
          push("room:setting", {"source": id, "room_id": room, "name": name, "color": color}, channel) |> ignore;
          /* Loopback. Update self state */
          dispatch(ReceiveRoomSetting(room, name, color));
        | None => state
        }
      | _ => state
      }};
let handleReceiveAvatarProfile= (avatar_id, name) =>{
      switch (state) {
      | Ready(state) => Ready({...state, name}))
      | _ => state
      };
      };
let handleSendAvatarProfile=>(name) =>  {
     let state = switch (state) {
      | Ready({ id, channel }) =>
        push("avatar:profile", {"source": id, "name": name}, channel) |> ignore;
        /* Loopback. Update self state */
        dispatch(ReceiveAvatarProfile(id, name));
      | _ => state
      }};
let handlUpdateText(input) =>{
      switch (state) {
      | Ready({ text } as state) =>{
        let state = Ready({
          ...state,
          text: input
        }));

        state}
      | _ => state
      }};
let handleReady = (id, channel, text, selected) =>{
        switch (selected) {
        | Some(room) =>
          push("room:message", {"source": id, "room_id": room, "body": text}, channel) |> ignore;
          /* Loopback. Update self state */
          dispatch(Receive(id, room, text));
          dispatch(UpdateText(""));
        | None => ()
        };};
let handleConnect = () => {
  let loungehello:string ="lounge:hello";
    let socket = initSocket("/socket")
                     |> connectSocket;
    let channel = socket |> initChannel(loungehello);
    let _ =
          channel
          |> putOn("room:message", (res: Abstract.any) => {
            let { source, room_id, body }: Decode.message = Decode.receive(res);
            dispatch(Receive(source, room_id, body));
          })
          |> putOn("room:setting", (res: Abstract.any) => {
            let { room_id, name, color }: Decode.setting = Decode.setting(res);
            dispatch(ReceiveRoomSetting(room_id, name, color));
          })
          |> putOn("avatar:profile", (res: Abstract.any) => {
            let { avatar_id, name }: Decode.profile = Decode.profile(res);
            dispatch(ReceiveAvatarProfile(avatar_id, name));
          })
          |> joinChannel
          |> putReceive("ok", (res: Abstract.any) => {
            let welcome: Decode.welcome = Decode.welcome(res);
            dispatch(Connected(welcome.id, welcome.name, socket, channel));
          });
};
  React.useEffect(()=>{
    dispatch(Connect)
    None
  });

    <div className="p-container">
      (switch (state) {
      | Ready({ name, rooms, available, entered, messages, text, selected }) =>
        <>
          <div className=(Room.roomClassName(selected, rooms))>
            <header className="c-header">{React.string("CizenChat")}</header>
            <div className="p-side-content">
              <InPlaceEdit
                name="user"
                text=name
                handleChange=(value => SendAvatarProfile(value) |> dispatch)
              />

              <button className="c-button" onClick=(_event => RoomCreate |> dispatch)>
                (React.string("Create Room"))
              </button>

              <RoomList
                title="Available Rooms"
                rooms=(Room.byIds(subtract(available, entered), rooms))
                handleSelect=(room => RoomEnter(room) |> dispatch)
              />

              <RoomList
                title="Joined Rooms"
                rooms=(Room.byIds(entered, rooms))
                handleSelect=(room => RoomSelect(room) |> dispatch)
              />
            </div>
          </div>
          <div className="p-chat">
            <div className="c-chat">
              (switch (selected) {
              | Some(room) =>
                <>
                  <div className="c-chat-header">
                    <InPlaceEdit
                      name="room-title"
                      text=(Room.getRoomName(room, rooms))
                      handleChange=(value => SendRoomSetting(Some(value), None) |> dispatch)
                    />
                    <ThemeChanger
                      handleChange=(color => SendRoomSetting(None, Some(color)) |> dispatch)
                    />
                  </div>
                  <MessageList messages=(Message.getMsg(room, messages)) />
                </>
              | None => <p>(React.string("Select or create a room"))</p>
              })
            </div>
            <div className="c-text-area-wrapper">
              <div className="c-text-area">
                <textarea
                  rows=1
                  placeholder="What's up?"
                  value=text
                  onKeyDown=(
                    event =>
                      if (ReactEvent.Keyboard.keyCode(event) === 13) {
                        ReactEvent.Keyboard.preventDefault(event);
                        dispatch(Send);
                      }
                  )
                  onChange=(
                    event =>
                      dispatch(UpdateText(ReactEvent.Form.target(event)##value))
                  )
                >
                </textarea>
              </div>
            </div>
          </div>
          <div className="p-avatars">
          </div>
        </>
      | _ => <div>(React.string("Connecting..."))</div>
      })
    </div>;
};
