open Message;

let component = ReasonReact.statelessComponent("MessageList");

let make = (~messages, _children) => {
  ...component,
  render: self =>
    <div className="c-messages">
      (
        messages
        |> Array.mapi((i: int, msg: Message.t) =>
             <div className="c-message" key=(string_of_int(i))>
               <b> (React.string(msg.body)) </b>
               <i> (React.string(" by " ++ msg.avatar_id)) </i>
             </div>
           )
        |> React.array
      )
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
        "messages": 'messages,
        "children": 'children,
      },
    ) =>
    make(~messages=reactProps##messages, reactProps##children)
  );
[@bs.obj]
external makeProps:
  (~children: 'children, ~messages: 'messages, unit) =>
  {
    .
    "messages": 'messages,
    "children": 'children,
  } =
  "";
