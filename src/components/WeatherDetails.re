let component = ReasonReact.statelessComponent("WeatherDetails");

let make = (~details, _children) => {
  ...component,
  render: self => <div> {ReasonReact.string(details)} </div>,
};