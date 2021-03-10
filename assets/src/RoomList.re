let component = ReasonReact.statelessComponent("RoomList");

let make = (~title, ~rooms, ~handleSelect, _children) => {
  ...component,
  render: self =>
    <div className="c-list">
      <div className="c-list-header"> (React.string(title)) </div>
      <div className="c-list-body">
        (
          rooms
          |> Array.map((room: Room.t) =>
               <div
                 className="c-list-item"
                 key=room.id
                 onClick=(_event => handleSelect(room.id))>
                 (React.string(room.name))
               </div>
             )
          |> React.array
        )
      </div>
    </div>,
};
/**
 * This is a wrapper created to let this component be used from the new React api.
 * Please convert this component to a [@react.component] function and then remove this wrapping code.
 */
let make =
  ReasonReactCompat.wrapReasonReactForReact(
    ~component,
    (
      reactProps: {
        .
        "handleSelect": 'handleSelect,
        "rooms": 'rooms,
        "title": 'title,
        "children": 'children,
      },
    ) =>
    make(
      ~handleSelect=reactProps##handleSelect,
      ~rooms=reactProps##rooms,
      ~title=reactProps##title,
      reactProps##children,
    )
  );
[@bs.obj]
external makeProps:
  (
    ~children: 'children,
    ~title: 'title,
    ~rooms: 'rooms,
    ~handleSelect: 'handleSelect,
    unit
  ) =>
  {
    .
    "handleSelect": 'handleSelect,
    "rooms": 'rooms,
    "title": 'title,
    "children": 'children,
  } =
  "";
