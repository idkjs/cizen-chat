type editing_state = {text: string};

type state =
  | Fixed
  | Editing(editing_state);

type action =
  | Toggle
  | Update(string);

let component = ReasonReact.reducerComponent("InPlaceEdit");

let make = (~name, ~text, ~handleChange, _children) => {
  ...component,
  initialState: () => Fixed,
  reducer: (action, state) =>
    switch (action) {
    | Toggle =>
      switch (state) {
      | Fixed => ReasonReact.Update(Editing({text: text}))
      | Editing({text}) => ReasonReact.Update(Fixed)
      }
    | Update(str) => ReasonReact.Update(Editing({text: str}))
    },
  render: self =>
    <div className=("c-iedit-" ++ name)>
      (
        switch (self.state) {
        | Fixed =>
          <>
            <span className=("c-iedit-" ++ name ++ "-content")>
              (React.string(text))
            </span>
            <span
              className=("c-iedit-" ++ name ++ "-action")
              onClick=(_event => Toggle |> self.send)>
              (React.string("Edit"))
            </span>
          </>
        | Editing({text as str}) =>
          <input
            value=str
            onKeyDown=(
              event =>
                if (ReactEvent.Keyboard.keyCode(event) === 13) {
                  ReactEvent.Keyboard.preventDefault(event);
                  handleChange(str);
                  Toggle |> self.send;
                }
            )
            onChange=(
              event =>
                Update(ReactEvent.Form.target(event)##value) |> self.send
            )
          />
        }
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
        "handleChange": 'handleChange,
        "text": 'text,
        "name": 'name,
        "children": 'children,
      },
    ) =>
    make(
      ~handleChange=reactProps##handleChange,
      ~text=reactProps##text,
      ~name=reactProps##name,
      reactProps##children,
    )
  );
[@bs.obj]
external makeProps:
  (
    ~children: 'children,
    ~name: 'name,
    ~text: 'text,
    ~handleChange: 'handleChange,
    unit
  ) =>
  {
    .
    "handleChange": 'handleChange,
    "text": 'text,
    "name": 'name,
    "children": 'children,
  } =
  "";
