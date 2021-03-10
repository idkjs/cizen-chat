[@react.component]
let make = (~handleChange) => {
  <div className="c-colors">
    <div
      onClick={_event => handleChange("red")}
      className="c-colors-item c-colors-item--red"
    />
    <div
      onClick={_event => handleChange("green")}
      className="c-colors-item c-colors-item--green"
    />
    <div
      onClick={_event => handleChange("blue")}
      className="c-colors-item c-colors-item--blue"
    />
    <div
      onClick={_event => handleChange("yellow")}
      className="c-colors-item c-colors-item--yellow"
    />
  </div>;
};
/**
 * This is a wrapper created to let this component be used from the new React api.
 * Please convert this component to a [@react.component] function and then remove this wrapping code.
 */;
// let make =
//   ReasonReactCompat.wrapReasonReactForReact(
//     ~component,
//     (
//       reactProps: {
//         .
//         "handleChange": 'handleChange,
//         "children": 'children,
//       },
//     ) =>
//     make(~handleChange=reactProps##handleChange, reactProps##children)
//   );
// [@bs.obj]
// external makeProps:
//   (~children: 'children, ~handleChange: 'handleChange, unit) =>
//   {
//     .
//     "handleChange": 'handleChange,
//     "children": 'children,
//   } =
//   "";
